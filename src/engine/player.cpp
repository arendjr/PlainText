#include "player.h"

#include <QTimerEvent>

#include "area.h"
#include "exit.h"
#include "race.h"
#include "realm.h"
#include "util.h"


Player::Player(uint id, Options options) :
    Character("player", id, options),
    m_regenerationInterval(0),
    m_admin(false),
    m_session(0) {

    setIndefiniteArticle("");
}

Player::~Player() {

    if (~options() & Copy) {
        Realm::instance()->unregisterPlayer(this);
    }
}

void Player::setName(const QString &newName) {

    Q_ASSERT(name().isEmpty());

    GameObject::setName(newName);

    if (~options() & Copy) {
        Realm::instance()->registerPlayer(this);
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
        m_regenerationInterval = startTimer(30000);
    } else {
        killTimer(m_regenerationInterval);
        m_regenerationInterval = 0;

        if (secondsStunned() > 0) {
            setLeaveOnActive(true);
        } else {
            leave(currentArea());
        }
    }
}

void Player::send(const QString &_message, Color color) {

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

void Player::enter(const GameObjectPtr &areaPtr) {

    Area *area = areaPtr.cast<Area *>();

    setCurrentArea(area);

    Util::sendOthers(area->players(), QString("%1 arrived.").arg(name()));

    area->addPlayer(this);

    look();
}

void Player::leave(const GameObjectPtr &areaPtr, const QString &exitName) {

    Area *area = areaPtr.cast<Area *>();

    area->removePlayer(this);

    QString text = (exitName.isEmpty() ?
                    QString("%1 left.").arg(name()) :
                    QString("%1 left to the %2.").arg(name(), exitName));
    Util::sendOthers(area->players(), text);
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
        text += QString("You see %1.\n").arg(Util::joinItems(area->npcs()));
    }

    if (area->items().length() > 0) {
        text += QString("You see %1.\n").arg(Util::joinItems(area->items()));
    }

    send(text);
}

void Player::die(const GameObjectPtr &attacker) {

    Q_UNUSED(attacker)

    Area *area = currentArea().cast<Area *>();
    GameObjectPtrList players = area->players();

    send(Util::colorize("You died.", Maroon));
    Util::sendOthers(players, Util::colorize(QString("%1 died.").arg(name()), Teal), this);

    GameObjectPtrList others = area->characters();
    others.removeOne(this);
    for (const GameObjectPtr &other : others) {
        other->invokeTrigger("oncharacterdied", this, attacker);
    }

    killAllTimers();
    clearEffects();
    clearModifiers();

    setHp(1);

    area->removePlayer(this);
    enter(race().cast<Race *>()->startingArea());
}

void Player::timerEvent(QTimerEvent *event) {

    if (event->timerId() == m_regenerationInterval) {
        adjustHp(qMax(stats().vitality / 15, 1));
        send("");
    } else {
        Character::timerEvent(event);
    }
}
