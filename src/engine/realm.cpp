#include "realm.h"

#include <cstring>

#include <QDir>

#include "character.h"


Realm *Realm::s_instance = 0;


Realm *Realm::instance() {

    if (s_instance == 0) {
        s_instance = new Realm();
        s_instance->init();
    }

    return s_instance;
}

void Realm::destroy() {

    delete s_instance;
    s_instance = 0;
}

void Realm::registerObject(GameObject *gameObject) {

    Q_ASSERT(gameObject);

    uint id = gameObject->id();
    m_objectMap.insert(id, gameObject);

    if (id >= m_nextId) {
        m_nextId = id + 1;
    }
}

void Realm::unregisterObject(GameObject *gameObject) {

    Q_ASSERT(gameObject);
    m_objectMap.remove(gameObject->id());
}

GameObject *Realm::getObject(const char *objectType, uint id) const {

    Q_ASSERT(objectType);

    if (m_objectMap.contains(id)) {
        GameObject *object = m_objectMap[id];
        Q_ASSERT(object);
        if (strcmp(object->objectType(), objectType) == 0) {
            return object;
        }
    }

    return 0;
}

void Realm::registerCharacter(Character *character) {

    Q_ASSERT(character);
    m_characterMap.insert(character->name(), character);
}

void Realm::unregisterCharacter(Character *character) {

    Q_ASSERT(character);
    m_characterMap.remove(character->name());
}

Character *Realm::getCharacter(const QString &name) const {

    if (m_characterMap.contains(name)) {
        return m_characterMap[name];
    }

    return 0;
}

uint Realm::uniqueObjectId() {

    return m_nextId++;
}

QString Realm::saveDirPath() {

    return QDir::homePath() + "/.mud";
}

QString Realm::saveObjectPath(const char *objectType, uint id) {

    return (saveDirPath() + "/%1.%2").arg(objectType).arg(id, 9, 10, QChar('0'));
}

Realm::Realm() :
    GameObject("realm", 0),
    m_initialized(false),
    m_nextId(1) {

    load(saveObjectPath(objectType(), id()));
}

Realm::~Realm() {

    foreach (GameObject *gameObject, m_objectMap) {
        Q_ASSERT(gameObject);
        delete gameObject;
    }

    save();
}

void Realm::init() {

    QDir dir(saveDirPath());
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        if (fileName.startsWith("realm.")) {
            continue;
        }
        GameObject::createFromFile(dir.path() + "/" + fileName);
    }

    foreach (GameObject *gameObject, m_objectMap) {
        Q_ASSERT(gameObject);
        gameObject->resolvePointers();
    }

    m_initialized = true;
}
