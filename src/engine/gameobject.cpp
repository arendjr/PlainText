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
#include "realm.h"
#include "util.h"


GameObject::GameObject(const char *objectType, uint id, QObject *parent) :
    QObject(parent),
    m_saved(false),
    m_objectType(objectType),
    m_id(id) {

    Q_ASSERT(objectType);

    if (m_id) {
        Realm::instance()->registerObject(this);
    }
}

GameObject::~GameObject() {

    if (m_id) {
        Realm::instance()->unregisterObject(this);
    }
}

bool GameObject::save() {

    if (m_saved) {
        return true;
    }

    QFile file(Realm::saveObjectPath(m_objectType, m_id));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file" << file.fileName() << "for writing.";
        return false;
    }

    file.write("{\n");

    bool firstProperty = true;
    int count = metaObject()->propertyCount(),
        offset = metaObject()->propertyOffset();
    for (int i = offset; i < count; i++) {
        QMetaProperty metaProperty = metaObject()->property(i);
        if (!metaProperty.isStored()) {
            continue;
        }

        if (firstProperty) {
            firstProperty = false;
        } else {
            file.write(",\n");
        }

        const char *name = metaProperty.name();
        file.write(QString("  \"%1\": ").arg(name).toUtf8());

        QStringList stringList;
        QVariantList variantList;
        switch (metaProperty.type()) {
            case QVariant::Bool:
                file.write(property(name).toBool() ? "true" : "false");
                break;
            case QVariant::Int:
                file.write(QString::number(property(name).toInt()).toUtf8());
                break;
            case QVariant::String:
                file.write(Util::jsString(property(name).toString()).toUtf8());
                break;
            case QVariant::StringList:
                stringList.clear();
                foreach (QString string, property(name).toStringList()) {
                    stringList << Util::jsString(string);
                }
                file.write("[ " + stringList.join(", ").toUtf8() + " ]");
                break;
            case QVariant::UserType:
                if (metaProperty.userType() == QMetaType::type("GameObjectPtr")) {
                    file.write("\"" + property(name).value<GameObjectPtr>().toString().toAscii() + "\"");
                    break;
                } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                    stringList.clear();
                    foreach (GameObjectPtr pointer, property(name).value<GameObjectPtrList>()) {
                        stringList << Util::jsString(pointer.toString());
                    }
                    file.write("[ " + stringList.join(", ").toAscii() + " ]");
                    break;
                } else if (metaProperty.userType() == QMetaType::type("Exit")) {
                    file.write(property(name).value<Exit>().toString().toUtf8());
                    break;
                } else if (metaProperty.userType() == QMetaType::type("ExitList")) {
                    stringList.clear();
                    foreach (Exit exit, property(name).value<ExitList>()) {
                        stringList << exit.toString();
                    }
                    file.write("[ " + stringList.join(", ").toUtf8() + " ]");
                    break;
                }
                // fall-through
            default:
                qDebug() << "Unknown type: " << metaProperty.type();
                break;
        }
    }

    file.write("\n}\n");

    m_saved = true;
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

    int count = metaObject()->propertyCount(),
        offset = metaObject()->propertyOffset();
    for (int i = offset; i < count; i++) {
        QMetaProperty metaProperty = metaObject()->property(i);
        if (!metaProperty.isStored()) {
            continue;
        }

        const char *name = metaProperty.name();
        if (!map.contains(name)) {
            continue;
        }

        ExitList exitList;
        QStringList stringList;
        GameObjectPtrList pointerList;
        switch (metaProperty.type()) {
            case QVariant::Bool:
            case QVariant::Int:
            case QVariant::String:
                setProperty(name, map[name]);
                break;
            case QVariant::StringList:
                stringList.clear();
                foreach (QVariant variant, map[name].toList()) {
                    stringList << variant.toString();
                }
                setProperty(name, stringList);
                break;
            case QVariant::UserType:
                if (metaProperty.userType() == QMetaType::type("GameObjectPtr")) {
                    setProperty(name, QVariant::fromValue(GameObjectPtr::fromString(map[name].toString())));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("GameObjectPtrList")) {
                    pointerList.clear();
                    foreach (QVariant variant, map[name].toList()) {
                        pointerList << GameObjectPtr::fromString(variant.toString());
                    }
                    setProperty(name, QVariant::fromValue(pointerList));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("Exit")) {
                    setProperty(name, QVariant::fromValue(Exit::fromVariantList(map[name].toList())));
                    break;
                } else if (metaProperty.userType() == QMetaType::type("ExitList")) {
                    exitList.clear();
                    foreach (QVariant variant, map[name].toList()) {
                        exitList << Exit::fromVariantList(variant.toList());
                    }
                    setProperty(name, QVariant::fromValue(exitList));
                    break;
                }
                // fall-through
            default:
                qDebug() << "Unknown type: " << metaProperty.type();
                break;
        }
    }

    m_saved = true;
    return true;
}

void GameObject::resolvePointers() {

    int count = metaObject()->propertyCount(),
        offset = metaObject()->propertyOffset();
    for (int i = offset; i < count; i++) {
        QMetaProperty metaProperty = metaObject()->property(i);
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
            } else if (metaProperty.userType() == QMetaType::type("Exit")) {
                Exit exit = property(name).value<Exit>();
                exit.resolvePointer();
                setProperty(name, QVariant::fromValue(exit));
            } else if (metaProperty.userType() == QMetaType::type("ExitList")) {
                ExitList exitList = property(name).value<ExitList>();
                for (int i = 0; i < exitList.length(); i++) {
                    exitList[i].resolvePointer();
                }
                setProperty(name, QVariant::fromValue(exitList));
            }
        }
    }
}

GameObject *GameObject::createByObjectType(const QString &objectType, uint id) {

    if (id == 0) {
        id = Realm::instance()->uniqueObjectId();
    }

    if (objectType == "area") {
        return new Area(id);
    } else if (objectType == "character") {
        return new Character(id);
    } else {
        throw BadGameObjectException(BadGameObjectException::UnknownGameObjectType);
    }
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

void GameObject::setModified() {

    if (Realm::instance()->isInitialized()) {
        m_saved = false;
    }
}
