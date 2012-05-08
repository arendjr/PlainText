#include "character.h"
#include "realm.h"


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
