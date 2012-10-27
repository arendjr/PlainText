#include "gameobject.h"

#include <cstring>

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMetaProperty>
#include <QMetaType>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

#include "qjson/json_driver.hh"

#include "character.h"
#include "class.h"
#include "container.h"
#include "conversionutil.h"
#include "deleteobjectevent.h"
#include "diskutil.h"
#include "exit.h"
#include "gameexception.h"
#include "gameobjectptr.h"
#include "group.h"
#include "item.h"
#include "player.h"
#include "race.h"
#include "realm.h"
#include "room.h"
#include "scriptengine.h"
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

    if (this != m_realm) {
        if (m_id == 0) {
            m_id = realm->uniqueObjectId();
        }
        if (~m_options & Copy) {
            m_realm->registerObject(this);
        }
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

bool GameObject::isClass() const {

    return strcmp(m_objectType, "class") == 0;
}

bool GameObject::isContainer() const {

    return strcmp(m_objectType, "container") == 0;
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

bool GameObject::isGroup() const {

    return strcmp(m_objectType, "group") == 0;
}

bool GameObject::isPlayer() const {

    return strcmp(m_objectType, "player") == 0;
}

bool GameObject::isRace() const {

    return strcmp(m_objectType, "race") == 0;
}

bool GameObject::isRealm() const {

    return strcmp(m_objectType, "realm") == 0;
}

bool GameObject::isRoom() const {

    return strcmp(m_objectType, "room") == 0;
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

QString GameObject::definiteName(const GameObjectPtrList &pool, int options) const {

    if (m_indefiniteArticle.isEmpty()) {
        return name();
    } else {
        int position = 0;
        int total = 0;
        for (const GameObjectPtr &other : pool) {
            if (other->name() == name()) {
                total++;

                if (other->id() == id()) {
                    position = total;
                }
            }
        }

        return QString(options & Capitalized ? "The " : "the ") +
               (total > 1 ? QString(Util::writtenPosition(position) + " ") : QLatin1String("")) +
               name();
    }
}

QString GameObject::indefiniteName(int options) const {

    if (m_indefiniteArticle.isEmpty()) {
        return name();
    } else {
        return (options & Capitalized ? Util::capitalize(m_indefiniteArticle) :
                                        m_indefiniteArticle) + " " + name();
    }
}

void GameObject::setName(const QString &name) {

    if (m_name != name) {
        m_name = name;

        setObjectName(name);
        setModified();

        changeName(m_name);
    }
}

void GameObject::setPlural(const QString &plural) {

    if (m_plural != plural) {
        m_plural = plural;

        setModified();
    }
}

void GameObject::setIndefiniteArticle(const QString &indefiniteArticle) {

    if (m_indefiniteArticle != indefiniteArticle) {
        m_indefiniteArticle = indefiniteArticle;

        setModified();
    }
}

void GameObject::setDescription(const QString &description) {

    if (m_description != description) {
        m_description = description;

        setModified();
    }
}

void GameObject::setData(const QVariantMap &data) {

    if (m_data != data) {
        m_data = data;

        setModified();
    }
}

void GameObject::setBoolData(const QString &name, bool value) {

    if (!m_data.contains(name) || m_data[name].type() != QVariant::Bool ||
        m_data[name].toBool() != value) {
        m_data[name] = value;

        setModified();
    }
}

void GameObject::setIntData(const QString &name, int value) {

    if (!m_data.contains(name) || m_data[name].type() != QVariant::Int ||
        m_data[name].toInt() != value) {
        m_data[name] = value;

        setModified();
    }
}

void GameObject::setStringData(const QString &name, const QString &value) {

    if (!m_data.contains(name) || m_data[name].type() != QVariant::String ||
        m_data[name].toString() != value) {
        m_data[name] = value;

        setModified();
    }
}

void GameObject::setGameObjectData(const QString &name, const GameObjectPtr &value) {

    if (!m_data.contains(name) || m_data[name].type() != QVariant::UserType ||
        m_data[name].userType() != QMetaType::type("GameObjectPtr") ||
        m_data[name].value<GameObjectPtr>() != value) {
        m_data[name] = QVariant::fromValue(value);

        setModified();
    }
}

void GameObject::setGameObjectListData(const QString &name, const GameObjectPtrList &value) {

    if (!m_data.contains(name) || m_data[name].type() != QVariant::UserType ||
        m_data[name].userType() != QMetaType::type("GameObjectPtrList") ||
        m_data[name].value<GameObjectPtrList>() != value) {
        m_data[name] = QVariant::fromValue(value);

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
                               GameObject *arg1, const GameObjectPtr &arg2,
                               const GameObjectPtrList &arg3, const QScriptValue &arg4) {

    ScriptEngine *engine = m_realm->scriptEngine();
    return invokeTrigger(triggerName,
                         engine->toScriptValue(arg1), engine->toScriptValue(arg2),
                         engine->toScriptValue(arg3), arg4);
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

void GameObject::send(const QString &message, int color) const {

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

QString GameObject::toJSON(Options options) const {

    QStringList dumpedProperties;
    if (~options & SkipId) {
        dumpedProperties << QString("  \"id\": %1").arg(m_id);
    }
    for (const QMetaProperty &metaProperty : storedMetaProperties()) {
        const char *name = metaProperty.name();

        QString propertyString = ConversionUtil::toJSON(property(name), options & IncludeTypeInfo);
        if (!propertyString.isNull()) {
            dumpedProperties << QString("  \"%1\": %2").arg(name, propertyString);
        }
    }
    return "{\n" + dumpedProperties.join(",\n") + "\n}";
}

bool GameObject::save() {

    if (m_deleted) {
        return QFile::remove(DiskUtil::gameObjectPath(m_objectType, m_id));
    } else {
        return DiskUtil::writeGameObject(m_objectType, m_id, toJSON(SkipId | IncludeTypeInfo));
    }
}

bool GameObject::load(const QString &path) {

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        throw GameException(GameException::CouldNotOpenGameObjectFile, path.toUtf8().constData());
    }

    bool error;
    JSonDriver driver;
    QVariantMap map = driver.parse(&file, &error).toMap();
    if (error) {
        throw GameException(GameException::CorruptGameObjectFile, path.toUtf8().constData());
    }

    for (const QMetaProperty &meta : storedMetaProperties()) {
        const char *name = meta.name();
        if (!map.contains(name)) {
            continue;
        }

        setProperty(name, ConversionUtil::fromVariant(meta.type(), meta.userType(), map[name]));
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

        if (~m_options & DontSave) {
            m_realm->addModifiedObject(this);
        }

        m_realm->enqueueEvent(new DeleteObjectEvent(this));
    }
}

GameObject *GameObject::createByObjectType(Realm *realm, const char *objectType, uint id,
                                           Options options) {

    switch (objectType[0]) {
        case 'c':
            if (strcmp(objectType + 1, "character" + 1) == 0) {
                return new Character(realm, id, options);
            } else if (strcmp(objectType + 1, "class" + 1) == 0) {
                return new Class(realm, id, options);
            } else if (strcmp(objectType + 1, "container" + 1) == 0) {
                return new Container(realm, id, options);
            }
            break;
        case 'e':
            if (strcmp(objectType + 1, "exit" + 1) == 0) {
                return new Exit(realm, id, options);
            }
            break;
        case 'g':
            if (strcmp(objectType + 1, "group" + 1) == 0) {
                return new Group(realm, id, options);
            }
            break;
        case 'i':
            if (strcmp(objectType + 1, "item" + 1) == 0) {
                return new Item(realm, id, options);
            }
            break;
        case 'p':
            if (strcmp(objectType + 1, "player" + 1) == 0) {
                return new Player(realm, id, options);
            }
            break;
        case 'r':
            if (strcmp(objectType + 1, "race" + 1) == 0) {
                return new Race(realm, id, options);
            } else if (strcmp(objectType + 1, "realm" + 1) == 0) {
                return new Realm(options);
            } else if (strcmp(objectType + 1, "room" + 1) == 0) {
                return new Room(realm, id, options);
            }
            break;
        case 's':
            if (strcmp(objectType + 1, "shield" + 1) == 0) {
                return new Shield(realm, id, options);
            }
            break;
        case 'w':
            if (strcmp(objectType + 1, "weapon" + 1) == 0) {
                return new Weapon(realm, id, options);
            }
            break;
        default:
            break;
    }

    throw GameException(GameException::UnknownGameObjectType);
}

GameObject *GameObject::createFromFile(Realm *realm, const QString &path) {

    QFileInfo fileInfo(path);
    QString fileName = fileInfo.fileName();
    QStringList components = fileName.split('.');
    if (components.length() != 2) {
        throw GameException(GameException::InvalidGameObjectFileName);
    }

    GameObject *gameObject = createByObjectType(realm, components[0].toAscii().constData(),
                                                       components[1].toInt());
    gameObject->load(path);
    return gameObject;
}

GameObject *GameObject::createCopy(const GameObject *other) {

    Q_ASSERT(other);
    GameObject *copy = createByObjectType(other->realm(), other->objectType(), other->id(), Copy);
    copy->m_deleted = other->m_deleted;

    for (const QMetaProperty &metaProperty : other->storedMetaProperties()) {
        const char *name = metaProperty.name();

        // game object pointers need to be unresolved to avoid them being
        // registrated in the other thread
        if (metaProperty.type() == QVariant::UserType) {
            if (metaProperty.userType() == QMetaType::type("GameObjectPtr")) {
                GameObjectPtr pointer = other->property(name).value<GameObjectPtr>();
                copy->setProperty(name, QVariant::fromValue(pointer.copyUnresolved()));
            } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                GameObjectPtrList list = other->property(name).value<GameObjectPtrList>();
                copy->setProperty(name, QVariant::fromValue(list.copyUnresolved()));
            } else {
                copy->setProperty(name, other->property(name));
            }
        } else {
            copy->setProperty(name, other->property(name));
        }
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

QList<QMetaProperty> GameObject::metaProperties() const {

    QList<QMetaProperty> properties;
    int count = metaObject()->propertyCount(),
        offset = GameObject::staticMetaObject.propertyOffset();
    for (int i = offset; i < count; i++) {
        properties << metaObject()->property(i);
    }
    return properties;
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

    if ((m_options & (Copy | DontSave)) == 0) {
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

void GameObject::changeName(const QString &newName) {

    int length = newName.length();
    if (length > 0 && !newName.startsWith('$')) {
        if (newName.endsWith("y") && length > 1 && !Util::isVowel(newName[length - 2])) {
            m_plural = newName.left(length - 1) + "ies";
        } else if (newName.endsWith("f")) {
            m_plural = newName.left(length - 1) + "ves";
        } else if (newName.endsWith("fe")) {
            m_plural = newName.left(length - 2) + "ves";
        } else if (newName.endsWith("s") || newName.endsWith("x") ||
                   newName.endsWith("sh") || newName.endsWith("ch")) {
            m_plural = newName.left(length - 2) + "es";
        } else if (newName.endsWith("ese")) {
            m_plural = newName;
        } else {
            m_plural = newName + "s";
        }

        if (Util::isVowel(newName[0])) {
            m_indefiniteArticle = "an";
        } else {
            m_indefiniteArticle = "a";
        }
    }
}
