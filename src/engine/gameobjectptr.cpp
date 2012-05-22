#include "gameobjectptr.h"

#include <algorithm>
#include <cstring>

#include <QMetaProperty>
#include <QStringList>

#include "badgameobjectexception.h"
#include "gameobject.h"
#include "realm.h"


GameObjectPtr::GameObjectPtr() :
    m_gameObject(0),
    m_objectType(0),
    m_id(0) {
}

GameObjectPtr::GameObjectPtr(GameObject *gameObject) :
    m_gameObject(gameObject),
    m_objectType(strdup(gameObject->objectType())),
    m_id(gameObject->id()) {
}

GameObjectPtr::GameObjectPtr(const char *objectType, uint id) :
    m_gameObject(0),
    m_objectType(strdup(objectType)),
    m_id(id) {

    if (Realm::instance()->isInitialized()) {
        resolve();
    }
}

GameObjectPtr::GameObjectPtr(const GameObjectPtr &other) :
    m_gameObject(other.m_gameObject),
    m_objectType(other.m_objectType),
    m_id(other.m_id) {

    if (m_objectType) {
        m_objectType = strdup(m_objectType);
    }
}

GameObjectPtr::~GameObjectPtr() {

    delete[] m_objectType;
}

GameObjectPtr &GameObjectPtr::operator=(GameObjectPtr other) {

    swap(*this, other);
    return *this;
}

bool GameObjectPtr::operator==(const GameObjectPtr &other) const {

    return m_id == other.m_id && m_gameObject == other.m_gameObject;
}

bool GameObjectPtr::operator==(const GameObject *other) const {

    Q_ASSERT(other);
    return m_gameObject == other;
}

bool GameObjectPtr::operator!=(const GameObjectPtr &other) const {

    return m_id != other.m_id || m_gameObject != other.m_gameObject;
}

bool GameObjectPtr::operator!=(const GameObject *other) const {

    Q_ASSERT(other);
    return m_gameObject != other;
}

void GameObjectPtr::resolve() throw (BadGameObjectException) {

    if (m_id == 0) {
        throw BadGameObjectException(BadGameObjectException::InvalidGameObjectPointer);
    }

    m_gameObject = Realm::instance()->getObject(m_objectType, m_id);
    if (m_gameObject == 0) {
        throw BadGameObjectException(BadGameObjectException::InvalidGameObjectPointer);
    }
    if (strcmp(m_objectType, "any") == 0) {
        delete[] m_objectType;
        m_objectType = strdup(m_gameObject->objectType());
    }
}

QString GameObjectPtr::toString() const {

    if (m_id == 0) {
        return "0";
    }

    return QString(m_objectType) + ":" + QString::number(m_id);
}

GameObjectPtr GameObjectPtr::fromString(const QString &string) throw (BadGameObjectException) {

    if (string == "0") {
        return GameObjectPtr();
    }

    QStringList components = string.split(':');
    if (components.length() != 2) {
        throw BadGameObjectException(BadGameObjectException::InvalidGameObjectPointer);
    }

    return GameObjectPtr(components[0].toAscii().constData(), components[1].toInt());
}

void swap(GameObjectPtr &first, GameObjectPtr &second) {

    std::swap(first.m_gameObject, second.m_gameObject);
    std::swap(first.m_objectType, second.m_objectType);
    std::swap(first.m_id, second.m_id);
}

QScriptValue GameObjectPtr::toScriptValue(QScriptEngine *engine, const GameObjectPtr &pointer) {

    Q_ASSERT(pointer.m_gameObject);
    return engine->newQObject(pointer.m_gameObject, QScriptEngine::QtOwnership,
                              QScriptEngine::ExcludeDeleteLater | QScriptEngine::PreferExistingWrapperObject);
}

void GameObjectPtr::fromScriptValue(const QScriptValue &object, GameObjectPtr &pointer) {

    pointer = GameObjectPtr("any", object.property("id").toUInt32());
}
