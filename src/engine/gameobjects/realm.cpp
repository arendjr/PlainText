#include "realm.h"

#include <QDebug>

#include "commandinterpreter.h"
#include "commandregistry.h"
#include "diskutil.h"
#include "player.h"
#include "triggerregistry.h"
#include "util.h"


static Realm *s_instance = nullptr;


#define super GameObject

Realm::Realm(Options options) :
    super(this, GameObjectType::Realm, 0, (Options) (options | DontRegister | NeverDelete)),
    m_initialized(false),
    m_nextId(1),
    m_timeIntervalId(0),
    m_gameThread(this),
    m_scriptEngine(nullptr) {

    if (~options & Copy) {
        s_instance = this;
    }

    m_commandRegistry = new CommandRegistry();

    m_commandInterpreter = new CommandInterpreter();
    m_commandInterpreter->setRegistry(m_commandRegistry);

    m_triggerRegistry = new TriggerRegistry();

    m_reservedNames << "all" << "down" << "east" << "north" << "northeast" << "northwest" << "out"
                    << "room" << "south" << "southeast" << "southwest" << "west";
    for (const QString &commandName : m_commandRegistry->commandNames()) {
        m_reservedNames.append(commandName);
    }

    for (int i = 0; i < GameObjectType::NumValues; i++) {
        m_numObjects[i] = 0;
    }

    load(DiskUtil::gameObjectPath("Realm", id()));
}

Realm::~Realm() {

    if (m_timeIntervalId) {
        stopInterval(m_timeIntervalId);
        m_timeIntervalId = 0;
    }

    m_gameThread.terminate();
    m_gameThread.wait();

    m_syncThread.terminate();
    m_logThread.terminate();

    m_syncThread.wait();
    m_logThread.wait();

    delete m_triggerRegistry;
    delete m_commandInterpreter;
    delete m_commandRegistry;
}

Realm *Realm::instance() {

    return s_instance;
}

void Realm::init() {

    for (const QString &fileName : DiskUtil::dataDirFileList()) {
        if (!fileName.startsWith("realm.")) {
            createFromFile(this, DiskUtil::dataDir() + "/" + fileName);
        }
    }

    for (GameObject *object : m_objectMap) {
        object->resolvePointers();
    }

    m_syncThread.start(QThread::LowestPriority);
    m_logThread.start(QThread::LowestPriority);

    m_initialized = true;

    super::init();

    for (GameObject *object : m_objectMap) {
        object->init();
    }

    m_timeIntervalId = startInterval(this, 150000);

    m_commandRegistry->moveToThread(&m_gameThread);
    m_commandInterpreter->moveToThread(&m_gameThread);
    m_triggerRegistry->moveToThread(&m_gameThread);

    m_gameThread.start(QThread::HighestPriority);
}

void Realm::registerObject(GameObject *gameObject) {

    Q_ASSERT(gameObject);

    uint id = gameObject->id();
    m_objectMap.insert(id, gameObject);

    int objectType = gameObject->objectType().value;
    switch (objectType) {
        case GameObjectType::Area:
            m_areas.append(gameObject);
            break;
        case GameObjectType::Room:
            m_rooms.append(gameObject);
            break;
        case GameObjectType::Class:
            m_classes.append(gameObject);
            break;
        case GameObjectType::Race:
            m_races.append(gameObject);
            break;
        default:
            m_numObjects[objectType]++;
            break;
    }

    if (id >= m_nextId) {
        m_nextId = id;
        do {
            m_nextId++;
        } while (m_objectMap.contains(m_nextId));
    }
}

void Realm::unregisterObject(GameObject *gameObject) {

    Q_ASSERT(gameObject);
    m_objectMap.remove(gameObject->id());

    int objectType = gameObject->objectType().value;
    if (m_numObjects[objectType] > 0) {
        m_numObjects[objectType]--;
    }
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

GameObject *Realm::getObject(const QString &objectType, uint id) {

    return getObject(GameObjectType::fromString(objectType), id);
}

GameObject *Realm::createObject(const QString &objectType) {

    return GameObject::createByObjectType(this, GameObjectType::fromString(objectType));
}

QVector<GameObject *> Realm::allObjects(GameObjectType objectType) const {

    QVector<GameObject *> objects;
    objects.reserve(m_numObjects[objectType.value]);
    if (objectType == GameObjectType::Unknown) {
        for (GameObject *object : m_objectMap) {
            objects.append(object);
        }
    } else {
        for (GameObject *object : m_objectMap) {
            if (object->objectType() == objectType) {
                objects.append(object);
            }
        }
    }
    return objects;
}

GameObjectPtrList Realm::players() const {

    GameObjectPtrList players;
    for (Player *player : m_playerMap) {
        players.append(player);
    }
    return players;
}

GameObjectPtrList Realm::onlinePlayers() const {

    GameObjectPtrList players;
    for (Player *player : m_playerMap) {
        if (player->session()) {
            players.append(player);
        }
    }
    return players;
}

void Realm::registerPlayer(Player *player) {

    Q_ASSERT(player);
    m_playerMap.insert(player->name(), player);
}

void Realm::unregisterPlayer(Player *player) {

    Q_ASSERT(player);
    m_playerMap.remove(player->name());
}

GameObject *Realm::getPlayer(const QString &name) const {

    if (m_playerMap.contains(name)) {
        return m_playerMap[name];
    }

    return 0;
}

void Realm::addReservedName(const QString &name) {

    QString userName = Util::validateUserName(name);
    if (!m_reservedNames.contains(userName)) {
        m_reservedNames.append(userName);
    }
}

void Realm::setDateTime(const QDateTime &dateTime) {

    if (m_dateTime != dateTime) {
        m_dateTime = dateTime;

        setModified();
    }
}

uint Realm::uniqueObjectId() {

    uint uniqueId = m_nextId;
    do {
        m_nextId++;
    } while (m_objectMap.contains(m_nextId));
    return uniqueId;
}

void Realm::enqueueEvent(Event *event) {

    m_gameThread.enqueueEvent(event);
}

void Realm::addModifiedObject(GameObject *object) {

    if (!m_initialized) {
        return;
    }

    m_modifiedObjects.insert(object);
}

void Realm::enqueueModifiedObjects() {

    for (GameObject *object : m_modifiedObjects) {
        m_syncThread.enqueueObject(object);
    }
    m_modifiedObjects.clear();
}

void Realm::enqueueLogMessage(LogMessage *message) {

    m_logThread.enqueueMessage(message);
}

void Realm::setScriptEngine(ScriptEngine *scriptEngine) {

    m_scriptEngine = scriptEngine;
}

void Realm::invokeTimer(int timerId) {

    if (timerId == m_timeIntervalId) {
        setDateTime(m_dateTime.addSecs(3600));

        emit hourPassed(m_dateTime);
        if (m_dateTime.time().hour() == 0) {
            emit dayPassed(m_dateTime);
        }
    } else {
        super::invokeTimer(timerId);
    }
}
