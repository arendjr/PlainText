#include "character.h"

#include "area.h"
#include "realm.h"
#include "util.h"


Character::Character(uint id, Options options) :
    GameObject("character", id, options),
    m_admin(false),
    m_session(0) {
}

Character::~Character() {

    if (~options() & Copy) {
        Realm::instance()->unregisterCharacter(this);
    }
}

void Character::setName(const QString &newName) {

    Q_ASSERT(name().isEmpty());

    GameObject::setName(newName);

    if (~options() & Copy) {
        Realm::instance()->registerCharacter(this);
    }
}

void Character::setPasswordHash(const QString &passwordHash) {

    if (m_passwordHash != passwordHash) {
        m_passwordHash = passwordHash;

        setModified();
    }
}

void Character::setCurrentArea(const GameObjectPtr &currentArea) {

    if (m_currentArea != currentArea) {
        m_currentArea = currentArea;

        setModified();
    }
}

void Character::addInventoryItem(const GameObjectPtr &item) {

    if (!m_inventory.contains(item)) {
        m_inventory << item;

        setModified();
    }
}

void Character::removeInventoryItem(const GameObjectPtr &item) {

    if (m_inventory.removeAll(item) > 0) {
        setModified();
    }
}

void Character::setInventory(const GameObjectPtrList &inventory) {

    if (m_inventory != inventory) {
        m_inventory = inventory;

        setModified();
    }
}

void Character::setAdmin(bool admin) {

    if (m_admin != admin) {
        m_admin = admin;

        setModified();
    }
}

void Character::setSession(Session *session) {

    m_session = session;
}

void Character::send(QString data) {

    write(data);
}

void Character::enter(const GameObjectPtr &areaPtr) {

    Area *area = areaPtr.cast<Area *>();
    Q_ASSERT(area);

    setCurrentArea(area);

    area->addCharacter(this);

    if (!area->name().isEmpty()) {
        send(Util::colorize("\n" + area->name() + "\n\n", Teal));
    }

    send(area->description());

    if (area->exits().length() > 0) {
        QStringList exitNames;
        foreach (const GameObjectPtr &exit, area->exits()) {
            exitNames << exit->name();
        }
        send(Util::colorize("Obvious exits: " + exitNames.join(", ") + ".", Green));
    }

    GameObjectPtrList others = area->characters();
    others.removeOne(this);
    if (others.length() > 0) {
        QStringList characterNames;
        foreach (const GameObjectPtr &otherPtr, others) {
            Character *other = otherPtr.cast<Character *>();
            Q_ASSERT(other);

            characterNames << other->name();

            other->send(QString("%1 arrived.").arg(name()));
        }
        send("You see " + Util::joinFancy(characterNames) + ".");
    }

    if (area->items().length() > 0) {
        send("You see " + Util::joinItems(area->items()) + ".");
    }
}

void Character::leave(const GameObjectPtr &areaPtr, const QString &exitName) {

    Area *area = areaPtr.cast<Area *>();
    Q_ASSERT(area);

    area->removeCharacter(this);

    foreach (const GameObjectPtr &otherPtr, area->characters()) {
        Character *other = otherPtr.cast<Character *>();
        Q_ASSERT(other);

        if (exitName.isEmpty()) {
            other->send(QString("%1 left.").arg(name()));
        } else {
            other->send(QString("%1 left to the %2.").arg(name(), exitName));
        }
    }
}

