#include "realm.h"

#include "diskutil.h"


static Realm *s_instance = nullptr;


#define super GameObject

Realm::Realm(Options options) :
    super(this, GameObjectType::Realm, 0, options),
    m_initialized(false),
    m_nextId(1) {

    s_instance = this;

    setAutoDelete(false);
}

Realm::~Realm() {
}

Realm *Realm::instance() {

    return s_instance;
}

void Realm::init() {

    load(DiskUtil::gameObjectPath(objectType().toString(), id()));

    for (const QString &fileName : DiskUtil::dataDirFileList()) {
        if (fileName.startsWith("room.") || fileName.startsWith("exit.")) {
            createFromFile(this, DiskUtil::dataDir() + "/" + fileName);
        }
    }

    for (GameObject *object : m_objectMap) {
        object->resolvePointers();
    }

    m_initialized = true;
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

GameObject *Realm::getObject(GameObjectType objectType, uint id) {

    if (id == 0) {
        if (objectType == GameObjectType::Unknown || this->objectType() == objectType) {
            return this;
        }
    }

    if (m_objectMap.contains(id)) {
        GameObject *object = m_objectMap[id];
        Q_ASSERT(object);
        if (objectType == GameObjectType::Unknown || object->objectType() == objectType) {
            return object;
        }
    }

    return nullptr;
}

GameObjectPtrList Realm::allObjects(GameObjectType objectType) const {

    GameObjectPtrList objects;
    for (GameObject *object : m_objectMap) {
        if (object->objectType() == objectType) {
            objects << object;
        }
    }
    return objects;
}

uint Realm::uniqueObjectId() {

    return m_nextId++;
}
