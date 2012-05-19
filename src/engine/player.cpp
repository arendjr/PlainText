#include "player.h"

#include "area.h"
#include "exit.h"
#include "realm.h"
#include "util.h"


Player::Player(uint id, Options options) :
    Character("player", id, options),
    m_hp(1),
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
}

void Player::send(QString message) {

    if (!message.endsWith("\n")) {
        message += "\n";
    }
    write(message);
}

void Player::enter(const GameObjectPtr &areaPtr) {

    Area *area = areaPtr.cast<Area *>();
    Q_ASSERT(area);

    setCurrentArea(area);

    Util::sendOthers(area->players(), QString("%1 arrived.").arg(name()));

    area->addPlayer(this);

    look();
}

void Player::leave(const GameObjectPtr &areaPtr, const QString &exitName) {

    Area *area = areaPtr.cast<Area *>();
    Q_ASSERT(area);

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
        foreach (const GameObjectPtr &exitPtr, area->exits()) {
            Exit *exit = exitPtr.cast<Exit *>();
            Q_ASSERT(exit);

            if (exit->isHidden()) {
                continue;
            }

            exitNames << exit->name();
        }
        text += Util::colorize("Obvious exits: " + exitNames.join(", ") + ".", Green) + "\n";
    }

    GameObjectPtrList others = area->players();
    others.removeOne(this);
    if (others.length() > 0) {
        QStringList playerNames;
        foreach (const GameObjectPtr &other, others) {
            playerNames << other->name();
        }
        text += "You see " + Util::joinFancy(playerNames) + ".\n";
    }

    if (area->npcs().length() > 0) {
        text += "You see " + Util::joinItems(area->npcs()) + ".\n";
    }

    if (area->items().length() > 0) {
        text += "You see " + Util::joinItems(area->items()) + ".\n";
    }

    send(text);
}
