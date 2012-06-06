#include "gameobject.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMetaProperty>
#include <QMetaType>
#include <QStringList>
#include <QTimerEvent>
#include <QVariantList>
#include <QVariantMap>

#include "qjson/json_driver.hh"

#include "area.h"
#include "character.h"
#include "characterstats.h"
#include "class.h"
#include "exit.h"
#include "gameexception.h"
#include "gameobjectptr.h"
#include "item.h"
#include "player.h"
#include "race.h"
#include "realm.h"
#include "scriptengine.h"
#include "scriptfunctionmap.h"
#include "util.h"


GameObject::GameObject(const char *objectType, uint id, Options options) :
    QObject(),
    m_autoDelete(true),
    m_objectType(objectType),
    m_id(id),
    m_options(options),
    m_numBulkModifications(0),
    m_deleted(false),
    m_intervalHash(0),
    m_timeoutHash(0) {

    Q_ASSERT(objectType);

    if (m_id && ~m_options & Copy) {
        Realm::instance()->registerObject(this);
    }
}

GameObject::~GameObject() {

    if (m_id && ~m_options & Copy) {
        Realm::instance()->unregisterObject(this);
    }

    foreach (GameObjectPtr *pointer, m_pointers) {
        *pointer = GameObjectPtr();
    }

    delete m_intervalHash;
    delete m_timeoutHash;
}

void GameObject::setName(const QString &name) {

    if (m_name != name) {
        m_name = name;

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
                               const QVariant &arg1, const QVariant &arg2,
                               const QVariant &arg3, const QVariant &arg4) {

    if (!m_triggers.contains(name)) {
        return true;
    }

    QVariantList arguments;
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

    QScriptValue returnValue = ScriptEngine::instance()->executeFunction(m_triggers[name], this, arguments);
    if (returnValue.isBool()) {
        return returnValue.toBool();
    } else {
        return true;
    }
}

bool GameObject::invokeTrigger(const QString &triggerName,
                               GameObject *arg1, const QVariant &arg2,
                               const QVariant &arg3, const QVariant &arg4) {

    return invokeTrigger(triggerName, QVariant::fromValue(GameObjectPtr(arg1)),
                         arg2, arg3, arg4);
}

void GameObject::send(const QString &message) {

    Q_UNUSED(message)
}

int GameObject::setInterval(const QScriptValue &function, int delay) {

    if (function.isString() || function.isFunction()) {
        if (!m_intervalHash) {
            m_intervalHash = new QHash<int, QScriptValue>;
        }

        int timerId = startTimer(delay);
        m_intervalHash->insert(timerId, function);
        return timerId;
    }
    return -1;
}

void GameObject::clearInterval(int timerId) {

    if (m_intervalHash) {
        killTimer(timerId);
        m_intervalHash->remove(timerId);
    }
}

int GameObject::setTimeout(const QScriptValue &function, int delay) {

    if (function.isString() || function.isFunction()) {
        if (!m_timeoutHash) {
            m_timeoutHash = new QHash<int, QScriptValue>;
        }

        int timerId = startTimer(delay);
        m_timeoutHash->insert(timerId, function);
        return timerId;
    }
    return -1;
}

void GameObject::clearTimeout(int timerId) {

    if (m_timeoutHash) {
        killTimer(timerId);
        m_timeoutHash->remove(timerId);
    }
}

bool GameObject::save() {

    if (m_deleted) {
        return QFile::remove(Realm::saveObjectPath(m_objectType, m_id));
    }

    const QString v("  \"%1\": %2");
    const QString o("  \"%1\": { %2 }");
    const QString l("  \"%1\": [ %2 ]");
    const QString k("%1: %2");

    QStringList dumpedProperties;
    foreach (const QMetaProperty &metaProperty, storedMetaProperties()) {
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
                foreach (const QString &string, property(name).toStringList()) {
                    stringList << Util::jsString(string);
                }
                if (!stringList.isEmpty()) {
                    dumpedProperties << l.arg(name, stringList.join(", "));
                }
                break;
            }
            case QVariant::DateTime: {
                dumpedProperties << v.arg(name).arg(property(name).toDateTime().toMSecsSinceEpoch());
            }
            case QVariant::UserType:
                if (metaProperty.userType() == QMetaType::type("GameObjectPtr")) {
                    dumpedProperties << v.arg(name, Util::jsString(property(name).value<GameObjectPtr>().toString()));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                    QStringList stringList;
                    foreach (const GameObjectPtr &pointer, property(name).value<GameObjectPtrList>()) {
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
                    foreach (const QString &key, functionMap.keys()) {
                        stringList << k.arg(Util::jsString(key), Util::jsString(functionMap[key].toString()));
                    }
                    if (!stringList.isEmpty()) {
                        dumpedProperties << o.arg(name, stringList.join(", "));
                    }
                    break;
                } else if (metaProperty.userType() == QMetaType::type("CharacterStats")) {
                    dumpedProperties << v.arg(name, property(name).value<CharacterStats>().toString());
                    break;
                }
                // fall-through
            default:
                qDebug() << "Unknown type: " << metaProperty.type();
        }
    }

    QFile file(Realm::saveObjectPath(m_objectType, m_id));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file" << file.fileName() << "for writing.";
        return false;
    }

    file.write("{\n" + dumpedProperties.join(",\n").toUtf8() + "\n}\n");
    return true;
}

bool GameObject::load(const QString &path) throw (GameException) {

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

    foreach (const QMetaProperty &metaProperty, storedMetaProperties()) {
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
                foreach (const QVariant &variant, map[name].toList()) {
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
                    setProperty(name, QVariant::fromValue(GameObjectPtr::fromString(map[name].toString())));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                    GameObjectPtrList pointerList;
                    foreach (const QVariant &variant, map[name].toList()) {
                        pointerList << GameObjectPtr::fromString(variant.toString());
                    }
                    setProperty(name, QVariant::fromValue(pointerList));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("ScriptFunctionMap")) {
                    ScriptFunctionMap functionMap;
                    QVariantMap variantMap = map[name].toMap();
                    foreach (const QString &key, variantMap.keys()) {
                        functionMap[key] = ScriptFunction::fromString(variantMap[key].toString());
                    }
                    setProperty(name, QVariant::fromValue(functionMap));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("CharacterStats")) {
                    setProperty(name, QVariant::fromValue(CharacterStats::fromVariantList(map[name].toList())));
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

    foreach (const QMetaProperty &metaProperty, storedMetaProperties()) {
        const char *name = metaProperty.name();
        if (metaProperty.type() == QVariant::UserType) {
            if (metaProperty.userType() == QMetaType::type("GameObjectPtr")) {
                GameObjectPtr pointer = property(name).value<GameObjectPtr>();
                pointer.resolve();
                setProperty(name, QVariant::fromValue(pointer));
            } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                GameObjectPtrList pointerList = property(name).value<GameObjectPtrList>();
                for (int i = 0; i < pointerList.length(); i++) {
                    pointerList[i].resolve();
                }
                setProperty(name, QVariant::fromValue(pointerList));
            }
        }
    }
}

void GameObject::setDeleted() {

    if (~m_options & Copy) {
        m_deleted = true;

        Realm::instance()->syncObject(this);

        deleteLater();
    }
}

GameObject *GameObject::createByObjectType(const QString &objectType, uint id, Options options) throw (GameException) {

    if (id == 0) {
        id = Realm::instance()->uniqueObjectId();
    }

    if (objectType == "area") {
        return new Area(id, options);
    } else if (objectType == "character") {
        return new Character(id, options);
    } else if (objectType == "class") {
        return new Class(id, options);
    } else if (objectType == "exit") {
        return new Exit(id, options);
    } else if (objectType == "item") {
        return new Item(id, options);
    } else if (objectType == "player") {
        return new Player(id, options);
    } else if (objectType == "race") {
        return new Race(id, options);
    } else if (objectType == "realm") {
        return new Realm(options);
    } else {
        throw GameException(GameException::UnknownGameObjectType);
    }
}

GameObject *GameObject::createCopy(const GameObject *other) {

    Q_ASSERT(other);
    GameObject *copy = createByObjectType(other->objectType(), other->id(), Copy);
    copy->m_deleted = other->m_deleted;

    foreach (const QMetaProperty &metaProperty, other->storedMetaProperties()) {
        const char *name = metaProperty.name();
        copy->setProperty(name, other->property(name));
    }

    return copy;
}

GameObject *GameObject::createFromFile(const QString &path) throw (GameException) {

    QFileInfo fileInfo(path);
    QString fileName = fileInfo.fileName();
    QStringList components = fileName.split('.');
    if (components.length() != 2) {
        throw GameException(GameException::InvalidGameObjectFileName);
    }

    GameObject *gameObject = createByObjectType(components[0], components[1].toInt());
    gameObject->load(path);
    return gameObject;
}

QScriptValue GameObject::toScriptValue(QScriptEngine *engine, GameObject *const &gameObject) {

    return engine->newQObject(gameObject, QScriptEngine::QtOwnership,
                              QScriptEngine::ExcludeDeleteLater | QScriptEngine::PreferExistingWrapperObject);
}

void GameObject::fromScriptValue(const QScriptValue &object, GameObject *&gameObject) {

    gameObject = qobject_cast<GameObject *>(object.toQObject());
    Q_ASSERT(gameObject);
}

void GameObject::startBulkModification() {

    m_numBulkModifications++;
}

void GameObject::commitBulkModification() {

    m_numBulkModifications--;
    setModified();
}

void GameObject::timerEvent(QTimerEvent *event) {

    int id = event->timerId();
    QScriptValue function;
    if (m_intervalHash) {
        function = m_intervalHash->value(id);
    }
    if (!function.isValid() && m_timeoutHash) {
        function = m_timeoutHash->value(id);
        if (function.isValid()) {
            killTimer(id);
        }
    }

    try {
        ScriptEngine *engine = ScriptEngine::instance();
        if (function.isString()) {
            engine->evaluate(function.toString());
        } else if (function.isFunction()) {
            function.call();
        }

        if (engine->hasUncaughtException()) {
            QScriptValue exception = engine->uncaughtException();
            qWarning() << "Script Exception: " + exception.toString();
        }
    } catch (const GameException &exception) {
        qWarning() << "Game Exception: " + QString(exception.what());
    }
}

void GameObject::setModified() {

    if (m_numBulkModifications == 0 && ~m_options & Copy) {
        Realm::instance()->syncObject(this);
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

    if (m_autoDelete && m_pointers.length() == 0) {
        setDeleted();
    }
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
