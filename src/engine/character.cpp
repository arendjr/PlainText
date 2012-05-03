#include "character.h"
#include "realm.h"


Character::Character(uint id, QObject *parent) :
    GameObject("character", id, parent),
    m_admin(false) {
}

Character::~Character() {

    Realm::instance()->unregisterCharacter(this);

    save();
}

void Character::setName(const QString &name) {

    Q_ASSERT(m_name.isEmpty());

    m_name = name;

    Realm::instance()->registerCharacter(this);

    setModified();
}

void Character::setPasswordHash(const QString &passwordHash) {

    m_passwordHash = passwordHash;

    setModified();
}

void Character::setCurrentArea(const GameObjectPtr &currentArea) {

    m_currentArea = currentArea;

    setModified();
}

void Character::setAdmin(bool admin) {

    m_admin = admin;

    setModified();
}
