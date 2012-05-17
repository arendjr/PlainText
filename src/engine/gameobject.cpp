#include "gameobject.h"

#include <exception>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMetaProperty>
#include <QMetaType>
#include <QStringList>
#include <QVariantMap>

#include "qjson/json_driver.hh"

#include "area.h"
#include "badgameobjectexception.h"
#include "character.h"
#include "exit.h"
#include "gameobjectptr.h"
#include "item.h"
#include "realm.h"
#include "scriptfunctionmap.h"
#include "util.h"


GameObject::GameObject(const char *objectType, uint id, Options options) :
    QObject(),
    m_objectType(objectType),
    m_id(id),
    m_options(options),
    m_deleted(false) {

    Q_ASSERT(objectType);

    if (m_id && ~m_options & Copy) {
        Realm::instance()->registerObject(this);
    }
}

GameObject::~GameObject() {

    if (m_id && ~m_options & Copy) {
        Realm::instance()->unregisterObject(this);
    }
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

void GameObject::setTriggers(const ScriptFunctionMap &triggers) {

    if (m_triggers != triggers) {
        m_triggers = triggers;

        setModified();
    }
}

bool GameObject::save() {

    if (m_deleted) {
        return QFile::remove(Realm::saveObjectPath(m_objectType, m_id));
    }

    const QString v("  \"%1\": %2");
    const QString o("  \"%1\": { %2 }");
    const QString l("  \"%1\": [ %2 ]");

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
                foreach (QString string, property(name).toStringList()) {
                    stringList << Util::jsString(string);
                }
                if (!stringList.isEmpty()) {
                    dumpedProperties << l.arg(name, stringList.join(", "));
                }
                break;
            }
            case QVariant::UserType:
                if (metaProperty.userType() == QMetaType::type("GameObjectPtr")) {
                    dumpedProperties << v.arg(name, Util::jsString(property(name).value<GameObjectPtr>().toString()));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                    QStringList stringList;
                    foreach (GameObjectPtr pointer, property(name).value<GameObjectPtrList>()) {
                        stringList << Util::jsString(pointer.toString());
                    }
                    if (!stringList.isEmpty()) {
                        dumpedProperties << l.arg(name, stringList.join(", "));
                    }
                    break;
                } else if (metaProperty.userType() == QMetaType::type("ScriptFunctionMap")) {
                    QStringList stringList;
                    ScriptFunctionMap functionMap = property(name).value<ScriptFunctionMap>();
                    foreach (QString key, functionMap.keys()) {
                        stringList << v.arg(key, Util::jsString(functionMap[key].toString()));
                    }
                    if (!stringList.isEmpty()) {
                        dumpedProperties << o.arg(name, stringList.join(", "));
                    }
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

bool GameObject::load(const QString &path) {

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        throw BadGameObjectException(BadGameObjectException::CouldNotOpenGameObjectFile);
    }

    bool error;
    JSonDriver driver;
    QVariantMap map = driver.parse(&file, &error).toMap();
    if (error) {
        throw BadGameObjectException(BadGameObjectException::CorruptGameObjectFile);
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
                foreach (QVariant variant, map[name].toList()) {
                    stringList << variant.toString();
                }
                setProperty(name, stringList);
                break;
            }
            case QVariant::UserType:
                if (metaProperty.userType() == QMetaType::type("GameObjectPtr")) {
                    setProperty(name, QVariant::fromValue(GameObjectPtr::fromString(map[name].toString())));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                    GameObjectPtrList pointerList;
                    foreach (QVariant variant, map[name].toList()) {
                        pointerList << GameObjectPtr::fromString(variant.toString());
                    }
                    setProperty(name, QVariant::fromValue(pointerList));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("ScriptFunctionMap")) {
                    ScriptFunctionMap functionMap;
                    QVariantMap variantMap = map[name].toMap();
                    foreach (QString key, variantMap.keys()) {
                        functionMap[key] = ScriptFunction::fromString(variantMap[key].toString());
                    }
                    setProperty(name, QVariant::fromValue(functionMap));
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

GameObject *GameObject::createByObjectType(const QString &objectType, uint id, Options options) {

    if (id == 0) {
        id = Realm::instance()->uniqueObjectId();
    }

    if (objectType == "area") {
        return new Area(id, options);
    } else if (objectType == "character") {
        return new Character(id, options);
    } else if (objectType == "exit") {
        return new Exit(id, options);
    } else if (objectType == "item") {
        return new Item(id, options);
    } else {
        throw BadGameObjectException(BadGameObjectException::UnknownGameObjectType);
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

GameObject *GameObject::createFromFile(const QString &path) {

    QFileInfo fileInfo(path);
    QString fileName = fileInfo.fileName();
    QStringList components = fileName.split('.');
    if (components.length() != 2) {
        throw BadGameObjectException(BadGameObjectException::InvalidGameObjectFileName);
    }

    GameObject *gameObject = createByObjectType(components[0], components[1].toInt());
    gameObject->load(path);
    return gameObject;
}

QScriptValue GameObject::toScriptValue(QScriptEngine *engine, GameObject *const &gameObject) {

    return engine->newQObject(gameObject);
}

void GameObject::fromScriptValue(const QScriptValue &object, GameObject *&gameObject) {

    gameObject = qobject_cast<GameObject *>(object.toQObject());
    Q_ASSERT(gameObject);
}

void GameObject::setModified() {

    if (~m_options & Copy) {
        Realm::instance()->syncObject(this);
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
