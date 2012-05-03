#include "character.h"
#include "realm.h"


Character::Character(uint id, QObject *parent) :
    GameObject("character", id, parent),
    m_admin(false),
    m_session(0) {
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
