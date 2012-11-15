#include "gameobject.h"

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

#include "conversionutil.h"
#include "diskutil.h"
#include "exit.h"
#include "gameexception.h"
#include "gameobjectptr.h"
#include "portal.h"
#include "realm.h"
#include "room.h"


GameObject::GameObject(Realm *realm, GameObjectType objectType, uint id, Options options) :
    QObject(),
    m_realm(realm),
    m_objectType(objectType),
    m_id(id),
    m_options(options),
    m_autoDelete(true),
    m_deleted(false),
    m_intervalHash(nullptr),
    m_timeoutHash(nullptr) {

    Q_ASSERT(objectType != GameObjectType::Unknown);

    if (this != m_realm) {
        if (m_id == 0) {
            m_id = realm->uniqueObjectId();
        }
        m_realm->registerObject(this);
    }
}

GameObject::~GameObject() {

    if (m_id) {
        m_realm->unregisterObject(this);
    }

    for (GameObjectPtr *pointer : m_pointers) {
        pointer->unresolve(false);
        *pointer = GameObjectPtr();
    }
}

bool GameObject::isExit() const {

    return m_objectType == GameObjectType::Exit;
}

bool GameObject::isPortal() const {

    return m_objectType == GameObjectType::Portal;
}

bool GameObject::isRealm() const {

    return m_objectType == GameObjectType::Realm;
}

bool GameObject::isRoom() const {

    return m_objectType == GameObjectType::Room;
}

void GameObject::setName(const QString &name) {

    if (m_name != name) {
        m_name = name;

        setObjectName(name);
        setModified();
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

QString GameObject::toJsonString(Options options) const {

    QStringList dumpedProperties;
    if (~options & SkipId) {
        dumpedProperties << QString("  \"id\": %1").arg(m_id);
    }
    for (const QMetaProperty &metaProperty : storedMetaProperties()) {
        const char *name = metaProperty.name();

        QString propertyString = ConversionUtil::toJsonString(property(name),
                                                              options & IncludeTypeInfo);
        if (!propertyString.isNull()) {
            dumpedProperties << QString("  \"%1\": %2").arg(name, propertyString);
        }
    }
    return "{\n" + dumpedProperties.join(",\n") + "\n}";
}

bool GameObject::save() {

    if (m_deleted) {
        return QFile::remove(DiskUtil::gameObjectPath(m_objectType.toString(), m_id));
    } else {
        return DiskUtil::writeGameObject(m_objectType.toString(), m_id,
                                         toJsonString(SkipId | IncludeTypeInfo));
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
                try {
                    pointer.resolve(m_realm);
                } catch (const GameException &exception) {
                    pointer = GameObjectPtr();
                }
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

    m_deleted = true;
}

GameObject *GameObject::createByObjectType(Realm *realm, GameObjectType objectType, uint id,
                                           Options options) {

    switch (objectType.value) {
        case GameObjectType::Exit:
            return new Exit(realm, id, options);
        case GameObjectType::Portal:
            return new Portal(realm, id, options);
        case GameObjectType::Realm:
            return new Realm(options);
        case GameObjectType::Room:
            return new Room(realm, id, options);
        case GameObjectType::Unknown:
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

    QString objectType = components[0].left(1).toUpper() + components[0].mid(1);
    GameObject *gameObject = createByObjectType(realm, GameObjectType::fromString(objectType),
                                                       components[1].toInt());
    gameObject->load(path);
    return gameObject;
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

    static QHash<int, QList<QMetaProperty> > storedProperties;
    if (!storedProperties.contains(m_objectType.intValue())) {
        QList<QMetaProperty> properties;
        int count = metaObject()->propertyCount(),
            offset = GameObject::staticMetaObject.propertyOffset();
        for (int i = offset; i < count; i++) {
            QMetaProperty metaProperty = metaObject()->property(i);
            if (metaProperty.isStored()) {
                if (isRoom() || isExit() || isPortal()) {
                    // specific optimization to avoid useless inflation of some objects
                    if (strcmp(metaProperty.name(), "plural") == 0 ||
                        strcmp(metaProperty.name(), "indefiniteArticle") == 0) {
                        continue;
                    }
                }
                properties << metaProperty;
            }
        }
        storedProperties[m_objectType.intValue()] = properties;
    }
    return storedProperties[m_objectType.intValue()];
}

void GameObject::setModified() {
}

void GameObject::setAutoDelete(bool autoDelete) {

    m_autoDelete = autoDelete;
}

void GameObject::registerPointer(GameObjectPtr *pointer) {

    Q_ASSERT(!m_pointers.contains(pointer));
    m_pointers.append(pointer);
}

void GameObject::unregisterPointer(GameObjectPtr *pointer) {

    int index = m_pointers.indexOf(pointer);
    Q_ASSERT(index > -1);
    m_pointers.remove(index);

    if (m_autoDelete && m_pointers.isEmpty()) {
        setDeleted();
    }
}
