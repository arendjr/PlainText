#include "gameobjectptr.h"

#include <algorithm>
#include <cstring>

#include <QMetaProperty>
#include <QStringList>

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

    m_gameObject->registerPointer(this);
}

GameObjectPtr::GameObjectPtr(const char *objectType, uint id) :
    m_gameObject(0),
    m_objectType(0),
    m_id(id) {

    if (objectType) {
        m_objectType = strdup(objectType);
    }

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

    if (m_gameObject) {
        m_gameObject->registerPointer(this);
    }
}

GameObjectPtr::~GameObjectPtr() {

    if (m_gameObject) {
        m_gameObject->unregisterPointer(this);
    }

    delete[] m_objectType;
}

bool GameObjectPtr::isNull() const {

    return m_id == 0;
}

GameObjectPtr &GameObjectPtr::operator=(const GameObjectPtr &other) {

    if (&other == this) {
        return *this;
    }

    if (m_gameObject) {
        m_gameObject->unregisterPointer(this);
    }

    delete[] m_objectType;

    m_gameObject = other.m_gameObject;
    m_objectType = other.m_objectType;
    m_id = other.m_id;

    if (m_objectType) {
        m_objectType = strdup(m_objectType);
    }

    if (m_gameObject) {
        m_gameObject->registerPointer(this);
    }

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

void GameObjectPtr::resolve() throw (GameException) {

    if (m_id == 0) {
        return;
    }

    m_gameObject = Realm::instance()->getObject(m_objectType, m_id);
    if (m_gameObject == 0) {
        throw GameException(GameException::InvalidGameObjectPointer, m_objectType, m_id);
    }
    if (!m_objectType) {
        m_objectType = strdup(m_gameObject->objectType());
    }

    m_gameObject->registerPointer(this);
}

QString GameObjectPtr::toString() const {

    if (m_id == 0) {
        return "0";
    }

    return QString(m_objectType) + ":" + QString::number(m_id);
}

GameObjectPtr GameObjectPtr::fromString(const QString &string) throw (GameException) {

    if (string == "0") {
        return GameObjectPtr();
    }

    QStringList components = string.split(':');
    if (components.length() != 2) {
        throw GameException(GameException::InvalidGameObjectPointer);
    }

    return GameObjectPtr(components[0].toAscii().constData(), components[1].toInt());
}

void swap(GameObjectPtr &first, GameObjectPtr &second) {

    if (first.m_gameObject) {
        first.m_gameObject->unregisterPointer(&first);
    }
    if (second.m_gameObject) {
        second.m_gameObject->unregisterPointer(&second);
    }

    std::swap(first.m_gameObject, second.m_gameObject);
    std::swap(first.m_objectType, second.m_objectType);
    std::swap(first.m_id, second.m_id);

    if (first.m_gameObject) {
        first.m_gameObject->registerPointer(&first);
    }
    if (second.m_gameObject) {
        second.m_gameObject->registerPointer(&second);
    }
}

QScriptValue GameObjectPtr::toScriptValue(QScriptEngine *engine, const GameObjectPtr &pointer) {

    if (pointer.m_gameObject) {
        return engine->newQObject(pointer.m_gameObject, QScriptEngine::QtOwnership,
                                  QScriptEngine::ExcludeDeleteLater | QScriptEngine::PreferExistingWrapperObject);
    } else {
        return engine->nullValue();
    }
}

void GameObjectPtr::fromScriptValue(const QScriptValue &object, GameObjectPtr &pointer) {

    if (object.isQObject()) {
        pointer = GameObjectPtr(0, object.property("id").toUInt32());
    } else {
        pointer = GameObjectPtr();
    }
}
