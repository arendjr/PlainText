#include "player.h"

#include "room.h"
#include "commandinterpreter.h"
#include "exit.h"
#include "realm.h"
#include "session.h"
#include "util.h"


#define super Character

Player::Player(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Player, id, options),
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
            leave(currentRoom());
        }
    }
}

void Player::send(const QString &_message, int color) const {

    QString message;
    if (_message.endsWith("\n") ||
        (_message.startsWith("{") && _message.endsWith("}"))) {
        message = _message;
    } else {
        message = _message + "\n";
    }

    if ((Color) color != Silver) {
        message = Util::colorize(message, (Color) color);
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

    Room *room = currentRoom().cast<Room *>();
    QString text;

    if (!room->name().isEmpty()) {
        text += "\n" + Util::colorize(room->name(), Teal) + "\n\n";
    }

    text += room->description() + "\n";

    if (room->exits().length() > 0) {
        QStringList exitNames;
        for (const GameObjectPtr &exitPtr : room->exits()) {
            Exit *exit = exitPtr.cast<Exit *>();

            if (exit->isHidden()) {
                continue;
            }

            exitNames << exit->name();
        }
        exitNames = Util::sortExitNames(exitNames);
        text += Util::colorize("Obvious exits: " + exitNames.join(", ") + ".", Green) + "\n";
    }

    GameObjectPtrList others = room->players();
    others.removeOne(this);
    if (others.length() > 0) {
        QStringList playerNames;
        for (const GameObjectPtr &other : others) {
            playerNames << other->name();
        }
        text += QString("You see %1.\n").arg(Util::joinFancy(playerNames));
    }

    if (room->npcs().length() > 0) {
        text += QString("You see %1.\n").arg(room->npcs().joinFancy());
    }

    if (room->items().length() > 0) {
        text += QString("You see %1.\n").arg(room->items().joinFancy());
    }

    send(text);
}

void Player::execute(const QString &command) {

    realm()->commandInterpreter()->execute(this, command);
}

void Player::quit() {

    if (m_session != nullptr) {
        if (secondsStunned() > 0) {
            send(QString("Please wait %1 seconds.").arg(secondsStunned()), Olive);
        } else {
            m_session->signOut();
            setSession(nullptr);
        }
    }
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

void Player::enteredRoom() {

    look();
}
