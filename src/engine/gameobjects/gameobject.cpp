#include "gameobject.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMetaProperty>
#include <QMetaType>
#include <QScriptValueIterator>
#include <QStack>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

#include "qjson/json_driver.hh"

#include "area.h"
#include "character.h"
#include "class.h"
#include "container.h"
#include "conversionutil.h"
#include "deleteobjectevent.h"
#include "diskutil.h"
#include "gameeventmultipliermap.h"
#include "gameeventobject.h"
#include "gameexception.h"
#include "gameobjectptr.h"
#include "group.h"
#include "item.h"
#include "player.h"
#include "point3d.h"
#include "portal.h"
#include "race.h"
#include "realm.h"
#include "room.h"
#include "scriptengine.h"
#include "shield.h"
#include "util.h"
#include "weapon.h"


QMap<QString, QScriptValue> GameObject::s_prototypeMap;


static int GameEventMultiplierMapType;
static int GameObjectPtrType;
static int GameObjectPtrListType;
static int Point3DType;
static int ScriptFunctionMapType;


GameObject::GameObject(Realm *realm, GameObjectType objectType, uint id, Options options) :
    QObject(),
    m_realm(realm),
    m_objectType(objectType),
    m_id(id),
    m_options((Options) (options & Copy ? options : options | AutoDelete)),
    m_deleted(false),
    m_intervalHash(nullptr),
    m_timeoutHash(nullptr) {

    Q_ASSERT(objectType != GameObjectType::Unknown);

    if (~options & DontRegister) {
        if (m_id == 0) {
            m_id = m_realm->uniqueObjectId();
        }
        if (~m_options & Copy) {
            m_realm->registerObject(this);
        }
    } else {
        if (m_id == 0) {
            GameEventMultiplierMapType = QMetaType::type("GameEventMultiplierMap");
            GameObjectPtrType = QMetaType::type("GameObjectPtr");
            GameObjectPtrListType = QMetaType::type("GameObjectPtrList");
            Point3DType = QMetaType::type("Point3D");
            ScriptFunctionMapType = QMetaType::type("ScriptFunctionMap");
        }
    }
}

GameObject::~GameObject() {

    if (m_id && ~m_options & Copy) {
        m_realm->unregisterObject(this);
    }

    for (GameObjectPtr *pointer : m_pointers) {
        pointer->unresolve(false);
    }
    m_pointers.clear();

    killAllTimers();
}

bool GameObject::isArea() const {

    return m_objectType == GameObjectType::Area;
}

bool GameObject::isClass() const {

    return m_objectType == GameObjectType::Class;
}

bool GameObject::isCharacter() const {

    return m_objectType == GameObjectType::Character || isPlayer();
}

bool GameObject::isContainer() const {

    return m_objectType == GameObjectType::Container;
}

bool GameObject::isEvent() const {

    return m_objectType == GameObjectType::Event;
}

bool GameObject::isItem() const {

    return m_objectType == GameObjectType::Item ||
           isCharacter() || isContainer() || isShield() || isWeapon();
}

bool GameObject::isGroup() const {

    return m_objectType == GameObjectType::Group;
}

bool GameObject::isPlayer() const {

    return m_objectType == GameObjectType::Player;
}

bool GameObject::isPortal() const {

    return m_objectType == GameObjectType::Portal;
}

bool GameObject::isRace() const {

    return m_objectType == GameObjectType::Race;
}

bool GameObject::isRealm() const {

    return m_objectType == GameObjectType::Realm;
}

bool GameObject::isRoom() const {

    return m_objectType == GameObjectType::Room;
}

bool GameObject::isShield() const {

    return m_objectType == GameObjectType::Shield;
}

bool GameObject::isWeapon() const {

    return m_objectType == GameObjectType::Weapon;
}

bool GameObject::hasStats() const {

    return isCharacter() || isShield() || isWeapon();
}

void GameObject::setName(const QString &name) {

    if (m_name != name) {
        m_name = name;

        setObjectName(name);
        setModified();

        changeName(m_name);
    }
}

QString GameObject::definiteName(const GameObjectPtrList &pool, int options) const {

    try {
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
                   (total > 1 ? QString(Util::writtenPosition(position) + " ") : QString("")) +
                   name();
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in GameObject::definiteName(): " << exception.what();
        return m_name;
    }
}

QString GameObject::indefiniteName(int options) const {

    if (m_indefiniteArticle.isEmpty()) {
        return name();
    } else if (isItem() &&
               static_cast<const Item *>(this)->flags() & ItemFlags::AlwaysUseDefiniteArticle) {
        return (options & Capitalized ? "The " : "the ") + name();
    } else {
        return (options & Capitalized ? Util::capitalize(m_indefiniteArticle) :
                                        m_indefiniteArticle) + " " + name();
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
        m_data[name].userType() != GameObjectPtrType ||
        m_data[name].value<GameObjectPtr>() != value) {
        m_data[name] = QVariant::fromValue(value);

        setModified();
    }
}

void GameObject::setGameObjectListData(const QString &name, const GameObjectPtrList &value) {

    if (!m_data.contains(name) || m_data[name].type() != QVariant::UserType ||
        m_data[name].userType() != GameObjectPtrListType ||
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
        arguments.append(arg1);

        if (arg2.isValid()) {
            arguments.append(arg2);

            if (arg3.isValid()) {
                arguments.append(arg3);

                if (arg4.isValid()) {
                    arguments.append(arg4);
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

bool GameObject::hasScriptMethod(const QString &methodName) {

    ScriptEngine *engine = m_realm->scriptEngine();
    QScriptValue scriptObject = engine->toScriptValue(this);

    QScriptValue method = scriptObject.prototype().property(methodName);
    return method.isFunction();
}

QScriptValue GameObject::invokeScriptMethod(const QString &methodName,
                                            const QScriptValue &arg1, const QScriptValue &arg2,
                                            const QScriptValue &arg3, const QScriptValue &arg4) {

    ScriptEngine *engine = m_realm->scriptEngine();
    QScriptValue scriptObject = engine->toScriptValue(this);

    QScriptValue method = scriptObject.prototype().property(methodName);
    if (!method.isFunction()) {
        return QScriptValue();
    }

    QScriptValueList arguments;
    if (arg1.isValid()) {
        arguments.append(arg1);

        if (arg2.isValid()) {
            arguments.append(arg2);

            if (arg3.isValid()) {
                arguments.append(arg3);

                if (arg4.isValid()) {
                    arguments.append(arg4);
                }
            }
        }
    }

    QScriptValue result = method.call(scriptObject, arguments);
    if (engine->hasUncaughtException()) {
        QScriptValue exception = engine->uncaughtException();
        qWarning() << "Script Exception: " << exception.toString().toUtf8().constData() << endl
                   << "While executing game object method: "
                   << methodName.toUtf8().constData() << endl
                   << "Backtrace:" << endl
                   << exception.property("backtrace").toString().toUtf8().constData();
    }
    return result;
}

QScriptValue GameObject::invokeScriptMethod(const QString &methodName,
                                            GameObject *arg1,
                                            const QScriptValue &arg2,
                                            const QScriptValue &arg3,
                                            const QScriptValue &arg4) {

    ScriptEngine *engine = m_realm->scriptEngine();
    return invokeScriptMethod(methodName, engine->toScriptValue(arg1), arg2, arg3, arg4);
}

QScriptValue GameObject::invokeScriptMethod(const QString &methodName,
                                            const GameObjectPtr &arg1,
                                            const QScriptValue &arg2,
                                            const QScriptValue &arg3,
                                            const QScriptValue &arg4) {

    ScriptEngine *engine = m_realm->scriptEngine();
    return invokeScriptMethod(methodName, engine->toScriptValue(arg1), arg2, arg3, arg4);
}

void GameObject::send(const QString &message, int color) const {

    Q_UNUSED(message)
    Q_UNUSED(color)
}

QString GameObject::nameAtStrength(double strength) {

    ScriptEngine *engine = m_realm->scriptEngine();
    QScriptValue scriptObject = engine->toScriptValue(this);

    QScriptValue method = scriptObject.prototype().property("nameAtStrength");
    if (method.isFunction()) {
        QScriptValueList arguments;
        arguments.append(strength);
        QScriptValue result = method.call(scriptObject, arguments);
        if (engine->hasUncaughtException()) {
            QScriptValue exception = engine->uncaughtException();
            qWarning() << "Script Exception: " << exception.toString().toUtf8().constData() << endl
                       << "While executing game object method:  nameAtStrength" << endl
                       << "Backtrace:" << endl
                       << exception.property("backtrace").toString().toUtf8().constData();
        }
        return result.toString();
    } else {
        if (strength >= 1.0) {
            return m_name;
        } else {
            return "something";
        }
    }
}

QString GameObject::lookAtBy(GameObject *character) {

    ScriptEngine *engine = m_realm->scriptEngine();
    QScriptValue scriptObject = engine->toScriptValue(this);

    QScriptValue method = scriptObject.prototype().property("lookAtBy");
    if (method.isFunction()) {
        QScriptValueList arguments;
        arguments.append(engine->toScriptValue(character));
        QScriptValue result = method.call(scriptObject, arguments);
        if (engine->hasUncaughtException()) {
            QScriptValue exception = engine->uncaughtException();
            qWarning() << "Script Exception: " << exception.toString().toUtf8().constData() << endl
                       << "While executing game object method:  lookAtBy" << endl
                       << "Backtrace:" << endl
                       << exception.property("backtrace").toString().toUtf8().constData();
        }
        return result.toString();
    } else {
        if (m_description.isEmpty()) {
            return QString("There's nothing special about the %1.").arg(name());
        } else {
            return m_description;
        }
    }
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

bool GameObject::canBeDeleted() {

    return ~m_options & NeverDelete;
}

void GameObject::setDeleted() {

    if (~m_options & Copy && ~m_options & NeverDelete && !m_deleted) {
        m_deleted = true;

        if (m_options & DontSave) {
            m_realm->enqueueEvent(new DeleteObjectEvent(m_id));
        } else {
            m_realm->addModifiedObject(this);
        }
    }
}

QString GameObject::toJsonString(Options options) const {

    QStringList dumpedProperties;
    if (~options & SkipId) {
        dumpedProperties.append(QString("  \"id\": %1").arg(m_id));
    }
    for (const QMetaProperty &metaProperty : storedMetaProperties()) {
        const char *name = metaProperty.name();

        QString jsonString = ConversionUtil::toJsonString(property(name),
                                                          (Options) (options & IncludeTypeInfo));
        if (!jsonString.isEmpty()) {
            dumpedProperties.append(QString("  \"%1\": %2").arg(name, jsonString));
        }
    }
    return "{\n" + dumpedProperties.join(",\n") + "\n}";
}

bool GameObject::save() {

    if (m_deleted) {
        bool result = QFile::remove(DiskUtil::gameObjectPath(m_objectType.toString(), m_id));

        m_realm->enqueueEvent(new DeleteObjectEvent(m_id));

        return result;
    } else {
        return DiskUtil::writeGameObject(m_objectType.toString(), m_id,
                                         toJsonString((Options) (SkipId | IncludeTypeInfo)));
    }
}

void GameObject::load(const QString &path) {

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        throw GameException(GameException::CouldNotOpenGameObjectFile, path);
    }

    loadJson(file.readAll());
}

void GameObject::loadJson(const QString &jsonString) {

    bool error;
    JSonDriver driver;
    QVariantMap map = driver.parse(jsonString, &error).toMap();
    if (error) {
        throw GameException(GameException::InvalidGameObjectJson, jsonString);
    }

    for (const QMetaProperty &meta : storedMetaProperties()) {
        const char *name = meta.name();
        if (!map.contains(name)) {
            continue;
        }

        meta.write(this, ConversionUtil::fromVariant(meta.type(), meta.userType(), map[name]));
    }
}

void GameObject::resolvePointers() {

    for (const QMetaProperty &metaProperty : storedMetaProperties()) {
        if (metaProperty.type() == QVariant::UserType) {
            if (metaProperty.userType() == GameObjectPtrType) {
                GameObjectPtr pointer;
                void *value = &pointer;
                qt_metacall(QMetaObject::ReadProperty, metaProperty.propertyIndex(), &value);
                try {
                    pointer.resolve(m_realm);
                } catch (const GameException &exception) {
                    Q_UNUSED(exception)
                    pointer = GameObjectPtr();
                }
                qt_metacall(QMetaObject::WriteProperty, metaProperty.propertyIndex(), &value);
            } else if (metaProperty.userType() == GameObjectPtrListType) {
                GameObjectPtrList list;
                void *value = &list;
                qt_metacall(QMetaObject::ReadProperty, metaProperty.propertyIndex(), &value);
                list.resolvePointers(m_realm);
                qt_metacall(QMetaObject::WriteProperty, metaProperty.propertyIndex(), &value);
            }
        }
    }
}

QVector<QMetaProperty> GameObject::metaProperties() const {

    QVector<QMetaProperty> properties;
    int count = metaObject()->propertyCount(),
        offset = GameObject::staticMetaObject.propertyOffset();
    for (int i = offset; i < count; i++) {
        properties << metaObject()->property(i);
    }
    return properties;
}

QVector<QMetaProperty> GameObject::storedMetaProperties() const {

    static QHash<int, QVector<QMetaProperty> > storedProperties;
    if (!storedProperties.contains(m_objectType.intValue())) {
        QVector<QMetaProperty> properties;
        int count = metaObject()->propertyCount(),
            offset = GameObject::staticMetaObject.propertyOffset();
        for (int i = offset; i < count; i++) {
            QMetaProperty metaProperty = metaObject()->property(i);
            if (metaProperty.isStored()) {
                properties << metaProperty;
            }
        }
        storedProperties[m_objectType.intValue()] = properties;
    }
    return storedProperties[m_objectType.intValue()];
}

GameObject *GameObject::createByObjectType(Realm *realm, GameObjectType objectType, uint id,
                                           Options options) {

    switch (objectType.value) {
        case GameObjectType::Area:
            return new Area(realm, id, options);
        case GameObjectType::Character:
            return new Character(realm, id, options);
        case GameObjectType::Class:
            return new Class(realm, id, options);
        case GameObjectType::Container:
            return new Container(realm, id, options);
        case GameObjectType::Event:
            return new GameEventObject(realm, id, options);
        case GameObjectType::Group:
            return new Group(realm, id, options);
        case GameObjectType::Item:
            return new Item(realm, id, options);
        case GameObjectType::Player:
            return new Player(realm, id, options);
        case GameObjectType::Portal:
            return new Portal(realm, id, options);
        case GameObjectType::Race:
            return new Race(realm, id, options);
        case GameObjectType::Realm:
            return new Realm(options);
        case GameObjectType::Room:
            return new Room(realm, id, options);
        case GameObjectType::Shield:
            return new Shield(realm, id, options);
        case GameObjectType::Weapon:
            return new Weapon(realm, id, options);
        case GameObjectType::Unknown:
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

    QString objectType = Util::capitalize(components[0]);
    GameObject *gameObject = createByObjectType(realm, GameObjectType::fromString(objectType),
                                                       components[1].toUInt());
    gameObject->load(path);
    return gameObject;
}

GameObject *GameObject::createCopy(GameObject *other) {

    Q_ASSERT(other);
    GameObject *copy = createByObjectType(other->realm(), other->objectType(), other->id(), Copy);
    copy->m_deleted = other->m_deleted;

    for (const QMetaProperty &metaProperty : other->storedMetaProperties()) {
        if (metaProperty.type() == QVariant::UserType) {
            if (metaProperty.userType() == GameEventMultiplierMapType) {
                GameEventMultiplierMap map;
                void *value = &map;
                other->qt_metacall(QMetaObject::ReadProperty, metaProperty.propertyIndex(), &value);
                copy->qt_metacall(QMetaObject::WriteProperty, metaProperty.propertyIndex(), &value);
            } else if (metaProperty.userType() == GameObjectPtrType) {
                GameObjectPtr pointer;
                void *value = &pointer;
                other->qt_metacall(QMetaObject::ReadProperty, metaProperty.propertyIndex(), &value);
                pointer.unresolve();
                copy->qt_metacall(QMetaObject::WriteProperty, metaProperty.propertyIndex(), &value);
            } else if (metaProperty.userType() == GameObjectPtrListType) {
                GameObjectPtrList list;
                void *value = &list;
                other->qt_metacall(QMetaObject::ReadProperty, metaProperty.propertyIndex(), &value);
                list.unresolvePointers();
                copy->qt_metacall(QMetaObject::WriteProperty, metaProperty.propertyIndex(), &value);
            } else if (metaProperty.userType() == Point3DType) {
                Point3D point;
                void *value = &point;
                other->qt_metacall(QMetaObject::ReadProperty, metaProperty.propertyIndex(), &value);
                copy->qt_metacall(QMetaObject::WriteProperty, metaProperty.propertyIndex(), &value);
            } else if (metaProperty.userType() == ScriptFunctionMapType) {
                ScriptFunctionMap map;
                void *value = &map;
                other->qt_metacall(QMetaObject::ReadProperty, metaProperty.propertyIndex(), &value);
                copy->qt_metacall(QMetaObject::WriteProperty, metaProperty.propertyIndex(), &value);
            } else {
                metaProperty.write(copy, metaProperty.read(other));
            }
        } else if (metaProperty.type() == QVariant::Map) {
            QVariantMap map;
            void *value = &map;
            other->qt_metacall(QMetaObject::ReadProperty, metaProperty.propertyIndex(), &value);
            copy->qt_metacall(QMetaObject::WriteProperty, metaProperty.propertyIndex(), &value);
        } else if (metaProperty.type() == QVariant::String) {
            QString string;
            void *value = &string;
            other->qt_metacall(QMetaObject::ReadProperty, metaProperty.propertyIndex(), &value);
            copy->qt_metacall(QMetaObject::WriteProperty, metaProperty.propertyIndex(), &value);
        } else {
            metaProperty.write(copy, metaProperty.read(other));
        }
    }

    return copy;
}

QScriptValue GameObject::toScriptValue(QScriptEngine *engine, GameObject *const &gameObject) {

    QScriptValue object = engine->newQObject(gameObject, QScriptEngine::QtOwnership,
                                             QScriptEngine::ExcludeChildObjects |
                                             QScriptEngine::ExcludeDeleteLater |
                                             QScriptEngine::PreferExistingWrapperObject);

    if (!gameObject) {
        return object;
    }

    const QMetaObject *metaObject = gameObject->metaObject();
    QString className(metaObject->className());

    if (!s_prototypeMap.contains(className)) {
        QScriptValue prototype = engine->evaluate(className);
        if (!prototype.isFunction()) {
            prototype = engine->evaluate(QString("function %1() {}; %1").arg(className));
        }

        QStack<QScriptValue> prototypeChain;
        prototypeChain.push(prototype);
        QString name(className);
        while (name != "GameObject") {
            metaObject = metaObject->superClass();
            name = metaObject->className();

            QScriptValue superPrototype = engine->evaluate(name);
            if (!superPrototype.isFunction()) {
                superPrototype = engine->evaluate(QString("function %1() {}; %1").arg(name));
            }
            prototypeChain.push(superPrototype);
        }

        QScriptValue superPrototype = prototypeChain.pop();
        while (!prototypeChain.isEmpty()) {
            QScriptValue prototype = prototypeChain.pop();
            QScriptValue superInstance = superPrototype.construct();
            superInstance.setProperty("constructor", prototype);

            QScriptValueIterator it(prototype.property("prototype"));
            while (it.hasNext()) {
                it.next();
                superInstance.setProperty(it.name(), it.value());
            }

            prototype.setProperty("prototype", superInstance);

            superPrototype = prototype;
        }

        s_prototypeMap[className] = prototype;
    }

    object.setPrototype(s_prototypeMap[className].construct());

    return object;
}

void GameObject::fromScriptValue(const QScriptValue &object, GameObject *&gameObject) {

    gameObject = qobject_cast<GameObject *>(object.toQObject());
    Q_ASSERT(gameObject);
}

void GameObject::clearPrototypeMap() {

    s_prototypeMap.clear();
}

bool GameObject::mayReferenceOtherProperties() const {

    return ~m_options & Copy && m_realm->isInitialized();
}

void GameObject::setModified() {

    if (~m_options & DontSave) {
        m_realm->addModifiedObject(this);
    }
}

void GameObject::setAutoDelete(bool autoDelete) {

    if (autoDelete) {
        m_options = (Options) (m_options | AutoDelete);
    } else {
        m_options = (Options) (m_options & ~AutoDelete);
    }
}

void GameObject::registerPointer(GameObjectPtr *pointer) {

    if (m_options & NeverDelete) {
        return;
    } 

    Q_ASSERT(!m_pointers.contains(pointer));
    m_pointers.append(pointer);
}

void GameObject::unregisterPointer(GameObjectPtr *pointer) {

    if (m_options & NeverDelete) {
        return;
    } 

    int index = m_pointers.indexOf(pointer);
    Q_ASSERT(index > -1);
    m_pointers.remove(index);

    if (m_options & AutoDelete && m_pointers.isEmpty()) {
        setDeleted();
    }
}

void GameObject::changeName(const QString &newName) {

    if (m_options & AutomaticNameForms) {
        int length = newName.length();
        if (length > 1 && !newName.startsWith('$')) {
            if (newName.endsWith("y") && !Util::isVowel(newName[length - 2])) {
                m_plural = newName.left(length - 1) + "ies";
            } else if (newName.endsWith("f")) {
                m_plural = newName.left(length - 1) + "ves";
            } else if (newName.endsWith("fe")) {
                m_plural = newName.left(length - 2) + "ves";
            } else if (newName.endsWith("s") || newName.endsWith("x") ||
                       newName.endsWith("sh") || newName.endsWith("ch")) {
                m_plural = newName + "es";
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
}
