#include "player.h"

#include "area.h"
#include "exit.h"
#include "realm.h"
#include "util.h"


#define super Character

Player::Player(Realm *realm, uint id, Options options) :
    super(realm, "player", id, options),
    m_regenerationIntervalId(0),
    m_admin(false),
    m_session(0) {

    setIndefiniteArticle("");
}

Player::~Player() {

    if (~options() & Copy) {
        realm()->unregisterPlayer(this);
    }
}

void Player::setPasswordSalt(const QString &passwordSalt) {

    if (m_passwordSalt != passwordSalt) {
        m_passwordSalt = passwordSalt;

        setModified();
    }
}

void Player::setPasswordHash(const QString &passwordHash) {

    if (m_passwordHash != passwordHash) {
        m_passwordHash = passwordHash;

        setModified();
    }
}

void Player::setAdmin(bool admin) {

    if (m_admin != admin) {
        m_admin = admin;

        setModified();
    }
}

void Player::setSession(Session *session) {

    m_session = session;

    if (m_session) {
        m_regenerationIntervalId = realm()->startInterval(this, 30000);
    } else {
        realm()->stopInterval(m_regenerationIntervalId);
        m_regenerationIntervalId = 0;

        if (secondsStunned() > 0) {
            setLeaveOnActive(true);
        } else {
            leave(currentArea());
        }
    }
}

void Player::send(const QString &_message, Color color) const {

    QString message;
    if (_message.endsWith("\n")) {
        message = _message;
    } else {
        message = _message + "\n";
    }

    if (color != Silver) {
        message = Util::colorize(message, color);
    }

    write(message);
}

void Player::sendSellableItemsList(const GameObjectPtrList &items) {

    QString message;
    for (const GameObjectPtr &item : items) {
        message += QString("  %1$%3\n")
                   .arg(item->name().leftJustified(30))
                   .arg(item.cast<Item *>()->cost());
    }
    write(message);
}

void Player::look() {

    Area *area = currentArea().cast<Area *>();
    QString text;

    if (!area->name().isEmpty()) {
        text += "\n" + Util::colorize(area->name(), Teal) + "\n\n";
    }

    text += area->description() + "\n";

    if (area->exits().length() > 0) {
        QStringList exitNames;
        for (const GameObjectPtr &exitPtr : area->exits()) {
            Exit *exit = exitPtr.cast<Exit *>();

            if (exit->isHidden()) {
                continue;
            }

            exitNames << exit->name();
        }
        exitNames = Util::sortExitNames(exitNames);
        text += Util::colorize("Obvious exits: " + exitNames.join(", ") + ".", Green) + "\n";
    }

    GameObjectPtrList others = area->players();
    others.removeOne(this);
    if (others.length() > 0) {
        QStringList playerNames;
        for (const GameObjectPtr &other : others) {
            playerNames << other->name();
        }
        text += QString("You see %1.\n").arg(Util::joinFancy(playerNames));
    }

    if (area->npcs().length() > 0) {
        text += QString("You see %1.\n").arg(area->npcs().joinFancy());
    }

    if (area->items().length() > 0) {
        text += QString("You see %1.\n").arg(area->items().joinFancy());
    }

    send(text);
}

void Player::invokeTimer(int timerId) {

    if (timerId == m_regenerationIntervalId) {
        adjustHp(qMax(stats().vitality / 15, 1));
        send("");
    } else {
        super::invokeTimer(timerId);
    }
}

void Player::changeName(const QString &newName) {

    super::changeName(newName);

    if (~options() & Copy) {
        realm()->registerPlayer(this);
    }
}

void Player::enteredArea() {

    look();
}
