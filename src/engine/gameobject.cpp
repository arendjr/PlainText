#include "gameobject.h"

#include <cstring>
#include <unistd.h>

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMetaProperty>
#include <QMetaType>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

#include "qjson/json_driver.hh"

#include "area.h"
#include "character.h"
#include "characterstats.h"
#include "class.h"
#include "combatmessage.h"
#include "deleteobjectevent.h"
#include "exit.h"
#include "gameexception.h"
#include "gameobjectptr.h"
#include "item.h"
#include "player.h"
#include "race.h"
#include "realm.h"
#include "scriptengine.h"
#include "scriptfunctionmap.h"
#include "shield.h"
#include "util.h"
#include "weapon.h"


GameObject::GameObject(Realm *realm, const char *objectType, uint id, Options options) :
    QObject(),
    m_realm(realm),
    m_objectType(objectType),
    m_id(id),
    m_options(options),
    m_autoDelete(true),
    m_deleted(false),
    m_intervalHash(nullptr),
    m_timeoutHash(nullptr) {

    Q_ASSERT(objectType);

    if (m_id && ~m_options & Copy) {
        m_realm->registerObject(this);
    }
}

GameObject::~GameObject() {

    if (m_id && ~m_options & Copy) {
        m_realm->unregisterObject(this);
    }

    for (GameObjectPtr *pointer : m_pointers) {
        pointer->unresolve(false);
        *pointer = GameObjectPtr();
    }

    killAllTimers();
}

bool GameObject::isArea() const {

    return strcmp(m_objectType, "area") == 0;
}

bool GameObject::isClass() const {

    return strcmp(m_objectType, "class") == 0;
}

bool GameObject::isExit() const {

    return strcmp(m_objectType, "exit") == 0;
}

bool GameObject::isItem() const {

    return strcmp(m_objectType, "item") == 0 || isCharacter() || isShield() || isWeapon();
}

bool GameObject::isCharacter() const {

    return strcmp(m_objectType, "character") == 0 || isPlayer();
}

bool GameObject::isPlayer() const {

    return strcmp(m_objectType, "player") == 0;
}

bool GameObject::isRace() const {

    return strcmp(m_objectType, "race") == 0;
}

bool GameObject::isShield() const {

    return strcmp(m_objectType, "shield") == 0;
}

bool GameObject::isWeapon() const {

    return strcmp(m_objectType, "weapon") == 0;
}

bool GameObject::hasStats() const {

    return isCharacter() || isShield() || isWeapon();
}

void GameObject::setName(const QString &name) {

    if (m_name != name) {
        m_name = name;

        setObjectName(name);
        setModified();
    }
}

void GameObject::setDescription(const QString &description) {

    if (m_description != description) {
        m_description = description;

        setModified();
    }
}

void GameObject::setTrigger(const QString &name, const ScriptFunction &function) {

    if (!m_triggers.contains(name) || m_triggers[name] != function) {
        m_triggers.insert(name, function);

        setModified();
    }
}

void GameObject::unsetTrigger(const QString &name) {

    if (m_triggers.remove(name) > 0) {
        setModified();
    }
}

void GameObject::setTriggers(const ScriptFunctionMap &triggers) {

    if (m_triggers != triggers) {
        m_triggers = triggers;

        setModified();
    }
}

bool GameObject::invokeTrigger(const QString &name,
                               const QScriptValue &arg1, const QScriptValue &arg2,
                               const QScriptValue &arg3, const QScriptValue &arg4) {

    if (!m_triggers.contains(name)) {
        return true;
    }

    QScriptValueList arguments;
    if (arg1.isValid()) {
        arguments << arg1;

        if (arg2.isValid()) {
            arguments << arg2;

            if (arg3.isValid()) {
                arguments << arg3;

                if (arg4.isValid()) {
                    arguments << arg4;
                }
            }
        }
    }

    ScriptEngine *engine = m_realm->scriptEngine();
    QScriptValue returnValue = engine->executeFunction(m_triggers[name], this, arguments);
    if (returnValue.isBool()) {
        return returnValue.toBool();
    } else {
        return true;
    }
}

bool GameObject::invokeTrigger(const QString &triggerName,
                               GameObject *arg1, const GameObjectPtr &arg2,
                               const QScriptValue &arg3, const QScriptValue &arg4) {

    ScriptEngine *engine = m_realm->scriptEngine();
    return invokeTrigger(triggerName,
                         engine->toScriptValue(arg1), engine->toScriptValue(arg2), arg3, arg4);
}

bool GameObject::invokeTrigger(const QString &triggerName,
                               GameObject *arg1, const GameObjectPtrList &arg2,
                               const QScriptValue &arg3, const QScriptValue &arg4) {

    ScriptEngine *engine = m_realm->scriptEngine();
    return invokeTrigger(triggerName,
                         engine->toScriptValue(arg1), engine->toScriptValue(arg2), arg3, arg4);
}

bool GameObject::invokeTrigger(const QString &triggerName,
                               GameObject *arg1, const QScriptValue &arg2,
                               const QScriptValue &arg3, const QScriptValue &arg4) {

    ScriptEngine *engine = m_realm->scriptEngine();
    return invokeTrigger(triggerName, engine->toScriptValue(arg1), arg2, arg3, arg4);
}

bool GameObject::invokeTrigger(const QString &triggerName,
                               const GameObjectPtr &arg1, const GameObjectPtr &arg2,
                               const QScriptValue &arg3, const QScriptValue &arg4) {

    ScriptEngine *engine = m_realm->scriptEngine();
    return invokeTrigger(triggerName,
                         engine->toScriptValue(arg1), engine->toScriptValue(arg2), arg3, arg4);
}

bool GameObject::invokeTrigger(const QString &triggerName,
                               const GameObjectPtr &arg1, const QScriptValue &arg2,
                               const QScriptValue &arg3, const QScriptValue &arg4) {

    ScriptEngine *engine = m_realm->scriptEngine();
    return invokeTrigger(triggerName, engine->toScriptValue(arg1), arg2, arg3, arg4);
}

void GameObject::send(const QString &message, Color color) const {

    Q_UNUSED(message)
    Q_UNUSED(color)
}

int GameObject::setInterval(const QScriptValue &function, int delay) {

    if (function.isString() || function.isFunction()) {
        if (!m_intervalHash) {
            m_intervalHash = new QHash<int, QScriptValue>;
        }

        int intervalId = m_realm->startInterval(this, delay);
        m_intervalHash->insert(intervalId, function);
        return intervalId;
    } else {
        return -1;
    }
}

void GameObject::clearInterval(int intervalId) {

    if (m_intervalHash) {
        m_realm->stopInterval(intervalId);
        m_intervalHash->remove(intervalId);
    }
}

int GameObject::setTimeout(const QScriptValue &function, int delay) {

    if (function.isString() || function.isFunction()) {
        if (!m_timeoutHash) {
            m_timeoutHash = new QHash<int, QScriptValue>;
        }

        int timerId = m_realm->startTimer(this, delay);
        m_timeoutHash->insert(timerId, function);
        return timerId;
    } else {
        return -1;
    }
}

void GameObject::clearTimeout(int timerId) {

    if (m_timeoutHash) {
        m_realm->stopTimer(timerId);
        m_timeoutHash->remove(timerId);
    }
}

void GameObject::init() {

    invokeTrigger("oninit");
}

GameObject *GameObject::copy() {

    GameObject *object = GameObject::createByObjectType(realm(), objectType());
    for (const QMetaProperty &metaProperty : storedMetaProperties()) {
        const char *name = metaProperty.name();
        object->setProperty(name, property(name));
    }
    object->init();
    return object;
}

bool GameObject::save() {

    if (m_deleted) {
        return QFile::remove(saveObjectPath(m_objectType, m_id));
    }

    const QString v("  \"%1\": %2");
    const QString o("  \"%1\": { %2 }");
    const QString l("  \"%1\": [ %2 ]");
    const QString k("%1: %2");

    QStringList dumpedProperties;
    for (const QMetaProperty &metaProperty : storedMetaProperties()) {
        const char *name = metaProperty.name();

        switch (metaProperty.type()) {
            case QVariant::Bool:
                dumpedProperties << v.arg(name, property(name).toBool() ? "true" : "false");
                break;
            case QVariant::Int:
                dumpedProperties << v.arg(name).arg(property(name).toInt());
                break;
            case QVariant::Double:
                dumpedProperties << v.arg(name).arg(property(name).toDouble());
                break;
            case QVariant::String:
                if (!property(name).toString().isEmpty()) {
                    dumpedProperties << v.arg(name, Util::jsString(property(name).toString()));
                }
                break;
            case QVariant::StringList: {
                QStringList stringList;
                for (const QString &string : property(name).toStringList()) {
                    stringList << Util::jsString(string);
                }
                if (!stringList.isEmpty()) {
                    dumpedProperties << l.arg(name, stringList.join(", "));
                }
                break;
            }
            case QVariant::DateTime: {
                dumpedProperties << v.arg(name).arg(property(name).toDateTime()
                                                    .toMSecsSinceEpoch());
                break;
            }
            case QVariant::UserType:
                if (metaProperty.userType() == QMetaType::type("GameObjectPtr")) {
                    dumpedProperties << v.arg(name, Util::jsString(property(name)
                                                    .value<GameObjectPtr>().toString()));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                    QStringList stringList;
                    for (const GameObjectPtr &pointer : property(name).value<GameObjectPtrList>()) {
                        if (pointer.isNull()) {
                            continue;
                        }
                        stringList << Util::jsString(pointer.toString());
                    }
                    if (!stringList.isEmpty()) {
                        dumpedProperties << l.arg(name, stringList.join(", "));
                    }
                    break;
                } else if (metaProperty.userType() == QMetaType::type("ScriptFunctionMap")) {
                    QStringList stringList;
                    ScriptFunctionMap functionMap = property(name).value<ScriptFunctionMap>();
                    for (const QString &key : functionMap.keys()) {
                        stringList << k.arg(Util::jsString(key),
                                            Util::jsString(functionMap[key].toString()));
                    }
                    if (!stringList.isEmpty()) {
                        dumpedProperties << o.arg(name, stringList.join(", "));
                    }
                    break;
                } else if (metaProperty.userType() == QMetaType::type("CharacterStats")) {
                    dumpedProperties << v.arg(name,
                                              property(name).value<CharacterStats>().toString());
                    break;
                } else if (metaProperty.userType() == QMetaType::type("CombatMessageList")) {
                    QStringList stringList;
                    for (const CombatMessage &message : property(name).value<CombatMessageList>()) {
                        stringList << message.toString();
                    }
                    if (!stringList.isEmpty()) {
                        dumpedProperties << l.arg(name, stringList.join(", "));
                    }
                    break;
                }
                // fall-through
            default:
                qDebug() << "Unknown type: " << metaProperty.type();
        }
    }

    QFile file(saveObjectPath(m_objectType, m_id));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file" << file.fileName() << "for writing.";
        return false;
    }

    file.write("{\n" + dumpedProperties.join(",\n").toUtf8() + "\n}\n");
    file.flush();
#ifdef Q_OS_LINUX
    fdatasync(file.handle());
#else
    fsync(file.handle());
#endif
    return true;
}

bool GameObject::load(const QString &path) {

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
       throw GameException(GameException::CouldNotOpenGameObjectFile);
    }

    bool error;
    JSonDriver driver;
    QVariantMap map = driver.parse(&file, &error).toMap();
    if (error) {
       throw GameException(GameException::CorruptGameObjectFile);
    }

    for (const QMetaProperty &metaProperty : storedMetaProperties()) {
        const char *name = metaProperty.name();
        if (!map.contains(name)) {
            continue;
        }

        switch (metaProperty.type()) {
            case QVariant::Bool:
            case QVariant::Int:
            case QVariant::Double:
            case QVariant::String:
                setProperty(name, map[name]);
                break;
            case QVariant::StringList: {
                QStringList stringList;
                for (const QVariant &variant : map[name].toList()) {
                    stringList << variant.toString();
                }
                setProperty(name, stringList);
                break;
            }
            case QVariant::DateTime:
                setProperty(name, QDateTime::fromMSecsSinceEpoch(map[name].toLongLong()));
                break;
            case QVariant::UserType:
                if (metaProperty.userType() == QMetaType::type("GameObjectPtr")) {
                    setProperty(name, QVariant::fromValue(GameObjectPtr::fromString(m_realm,
                                                                                    map[name]
                                                                                    .toString())));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                    GameObjectPtrList pointerList;
                    for (const QVariant &variant : map[name].toList()) {
                        pointerList << GameObjectPtr::fromString(m_realm, variant.toString());
                    }
                    setProperty(name, QVariant::fromValue(pointerList));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("ScriptFunctionMap")) {
                    ScriptFunctionMap functionMap;
                    QVariantMap variantMap = map[name].toMap();
                    for (const QString &key : variantMap.keys()) {
                        functionMap[key] = ScriptFunction::fromString(variantMap[key].toString());
                    }
                    setProperty(name, QVariant::fromValue(functionMap));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("CharacterStats")) {
                    setProperty(name,
                                QVariant::fromValue(CharacterStats::fromVariantList(map[name]
                                                                                    .toList())));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("CombatMessageList")) {
                    CombatMessageList messageList;
                    for (const QVariant &variant : map[name].toList()) {
                        messageList << CombatMessage::fromVariantList(variant.toList());
                    }
                    setProperty(name, QVariant::fromValue(messageList));
                    break;
                }
                // fall-through
            default:
                qDebug() << "Unknown type: " << metaProperty.type();
        }
    }

    return true;
}

void GameObject::resolvePointers() {

    for (const QMetaProperty &metaProperty : storedMetaProperties()) {
        if (metaProperty.type() == QVariant::UserType) {
            const char *name = metaProperty.name();
            if (metaProperty.userType() == QMetaType::type("GameObjectPtr")) {
                GameObjectPtr pointer = property(name).value<GameObjectPtr>();
                pointer.resolve(m_realm);
                setProperty(name, QVariant::fromValue(pointer));
            } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                GameObjectPtrList pointerList = property(name).value<GameObjectPtrList>();
                pointerList.resolvePointers(m_realm);
                setProperty(name, QVariant::fromValue(pointerList));
            }
        }
    }
}

void GameObject::setDeleted() {

    if (~m_options & Copy) {
        m_deleted = true;

        m_realm->addModifiedObject(this);

        m_realm->enqueueEvent(new DeleteObjectEvent(this));
    }
}

GameObject *GameObject::createByObjectType(Realm *realm, const QString &objectType, uint id,
                                           Options options) {

    if (id == 0) {
        id = realm->uniqueObjectId();
    }

    if (objectType == "area") {
        return new Area(realm, id, options);
    } else if (objectType == "character") {
        return new Character(realm, id, options);
    } else if (objectType == "class") {
        return new Class(realm, id, options);
    } else if (objectType == "exit") {
        return new Exit(realm, id, options);
    } else if (objectType == "item") {
        return new Item(realm, id, options);
    } else if (objectType == "player") {
        return new Player(realm, id, options);
    } else if (objectType == "race") {
        return new Race(realm, id, options);
    } else if (objectType == "realm") {
        return new Realm(options);
    } else if (objectType == "shield") {
        return new Shield(realm, id, options);
    } else if (objectType == "weapon") {
        return new Weapon(realm, id, options);
    } else {
       throw GameException(GameException::UnknownGameObjectType);
    }
}

GameObject *GameObject::createFromFile(Realm *realm, const QString &path) {

    QFileInfo fileInfo(path);
    QString fileName = fileInfo.fileName();
    QStringList components = fileName.split('.');
    if (components.length() != 2) {
        throw GameException(GameException::InvalidGameObjectFileName);
    }

    GameObject *gameObject = createByObjectType(realm, components[0], components[1].toInt());
    gameObject->load(path);
    return gameObject;
}

GameObject *GameObject::createCopy(const GameObject *other) {

    Q_ASSERT(other);
    GameObject *copy = createByObjectType(other->realm(), other->objectType(), other->id(), Copy);
    copy->m_deleted = other->m_deleted;

    for (const QMetaProperty &metaProperty : other->storedMetaProperties()) {
        const char *name = metaProperty.name();
        QVariant value = other->property(name);

        // game object pointers need to be unresolved to avoid them being
        // registrated in the other thread
        if (metaProperty.type() == QVariant::UserType) {
            if (metaProperty.userType() == QMetaType::type("GameObjectPtr")) {
                GameObjectPtr pointer = value.value<GameObjectPtr>();
                pointer.unresolve();
                value = QVariant::fromValue(pointer);
            } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                GameObjectPtrList pointerList = value.value<GameObjectPtrList>();
                pointerList.unresolvePointers();
                value = QVariant::fromValue(pointerList);
            }
        }

        copy->setProperty(name, value);
    }

    return copy;
}

QScriptValue GameObject::toScriptValue(QScriptEngine *engine, GameObject *const &gameObject) {

    return engine->newQObject(gameObject, QScriptEngine::QtOwnership,
                              QScriptEngine::ExcludeDeleteLater |
                              QScriptEngine::PreferExistingWrapperObject);
}

void GameObject::fromScriptValue(const QScriptValue &object, GameObject *&gameObject) {

    gameObject = qobject_cast<GameObject *>(object.toQObject());
    Q_ASSERT(gameObject);
}

QList<QMetaProperty> GameObject::storedMetaProperties() const {

    QList<QMetaProperty> properties;
    int count = metaObject()->propertyCount(),
        offset = GameObject::staticMetaObject.propertyOffset();
    for (int i = offset; i < count; i++) {
        QMetaProperty metaProperty = metaObject()->property(i);
        if (metaProperty.isStored()) {
            properties << metaProperty;
        }
    }
    return properties;
}

void GameObject::invokeTimer(int timerId) {

    QScriptValue function;
    if (m_intervalHash) {
        function = m_intervalHash->value(timerId);
    }
    if (!function.isValid() && m_timeoutHash) {
        function = m_timeoutHash->value(timerId);
    }

    ScriptEngine *scriptEngine = m_realm->scriptEngine();
    if (function.isString()) {
        scriptEngine->evaluate(function.toString());
    } else if (function.isFunction()) {
        function.call(scriptEngine->toScriptValue(this));
    }

    if (scriptEngine->hasUncaughtException()) {
        QScriptValue exception = scriptEngine->uncaughtException();
        qWarning() << "Script Exception: " << exception.toString().toUtf8().constData() << endl
                   << "While executing function: " << function.toString().toUtf8().constData();
        scriptEngine->evaluate("");
    }
}

void GameObject::killAllTimers() {

    if (m_intervalHash) {
        for (int id : m_intervalHash->keys()) {
            m_realm->stopInterval(id);
        }
        delete m_intervalHash;
        m_intervalHash = nullptr;
    }
    if (m_timeoutHash) {
        for (int id : m_timeoutHash->keys()) {
            m_realm->stopTimer(id);
        }
        delete m_timeoutHash;
        m_timeoutHash = nullptr;
    }
}

bool GameObject::mayReferenceOtherProperties() const {

    return ~m_options & Copy && m_realm->isInitialized();
}

void GameObject::setModified() {

    if (~m_options & Copy) {
        m_realm->addModifiedObject(this);
    }
}

void GameObject::setAutoDelete(bool autoDelete) {

    m_autoDelete = autoDelete;
}

void GameObject::registerPointer(GameObjectPtr *pointer) {

    Q_ASSERT(!m_pointers.contains(pointer));
    m_pointers.append(pointer);
}

void GameObject::unregisterPointer(GameObjectPtr *pointer) {

    Q_ASSERT(m_pointers.contains(pointer));
    m_pointers.removeOne(pointer);

    if (m_autoDelete && m_pointers.isEmpty()) {
        setDeleted();
    }
}

QString GameObject::saveDirPath() {

    return QDir::homePath() + "/.mud";
}

QString GameObject::saveObjectPath(const char *objectType, uint id) {

    return QString(saveDirPath() + "/%1.%2").arg(objectType).arg(id, 9, 10, QChar('0'));
}
