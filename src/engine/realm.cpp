#include "realm.h"

#include <cstring>

#include "commandinterpreter.h"
#include "commandregistry.h"
#include "diskutil.h"
#include "player.h"
#include "util.h"


static Realm *s_instance = nullptr;


#define super GameObject

Realm::Realm(Options options) :
    super(this, GameObjectType::Realm, 0, options),
    m_initialized(false),
    m_nextId(1),
    m_timeIntervalId(0),
    m_gameThread(this),
    m_scriptEngine(nullptr) {

    if (~options & Copy) {
        s_instance = this;
    }

    setAutoDelete(false);

    m_commandRegistry = new CommandRegistry(this);

    m_commandInterpreter = new CommandInterpreter(this);
    m_commandInterpreter->setRegistry(m_commandRegistry);

    m_reservedNames << "all" << "down" << "east" << "north" << "northeast" << "northwest" << "out"
                    << "room" << "south" << "southeast" << "southwest" << "west";
    for (const QString &commandName : m_commandRegistry->commandNames()) {
        m_reservedNames << commandName;
    }
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
}

Realm *Realm::instance() {

    return s_instance;
}

void Realm::init() {

    load(DiskUtil::gameObjectPath(objectType().toString(), id()));

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

    m_gameThread.start(QThread::HighestPriority);
}

void Realm::registerObject(GameObject *gameObject) {

    Q_ASSERT(gameObject);

    uint id = gameObject->id();
    m_objectMap.insert(id, gameObject);

    if (gameObject->isRace()) {
        m_races.append(gameObject);
    } else if (gameObject->isClass()) {
        m_classes.append(gameObject);
    }

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

GameObject *Realm::getObject(const QString &objectType, uint id) {

    return getObject(GameObjectType::fromString(Util::capitalize(objectType)), id);
}

GameObject *Realm::createObject(const QString &objectType) {

    return GameObject::createByObjectType(this,
                                          GameObjectType::fromString(Util::capitalize(objectType)));
}

GameObjectPtrList Realm::players() const {

    GameObjectPtrList players;
    for (Player *player : m_playerMap) {
        players << player;
    }
    return players;
}

GameObjectPtrList Realm::onlinePlayers() const {

    GameObjectPtrList players;
    for (Player *player : m_playerMap) {
        if (player->session()) {
            players << player;
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

Player *Realm::getPlayer(const QString &name) const {

    if (m_playerMap.contains(name)) {
        return m_playerMap[name];
    }

    return 0;
}

GameObjectPtrList Realm::allRooms() const {

    GameObjectPtrList areas;
    for (GameObject *object : m_objectMap) {
        if (object->isRoom()) {
            areas << object;
        }
    }
    return areas;
}

GameObjectPtrList Realm::allExits() const {

    GameObjectPtrList exits;
    for (GameObject *object : m_objectMap) {
        if (object->isExit()) {
            exits << object;
        }
    }
    return exits;
}

void Realm::addReservedName(const QString &name) {

    QString userName = Util::validateUserName(name);
    if (!m_reservedNames.contains(userName)) {
        m_reservedNames << userName;
    }
}

void Realm::setDateTime(const QDateTime &dateTime) {

    if (m_dateTime != dateTime) {
        m_dateTime = dateTime;

        setModified();
    }
}

uint Realm::uniqueObjectId() {

    return m_nextId++;
}

void Realm::enqueueEvent(Event *event) {

    m_gameThread.enqueueEvent(event);
}

void Realm::addModifiedObject(GameObject *object) {

    if (!m_initialized) {
        return;
    }

    if (!m_modifiedObjects.contains(object)) {
        m_modifiedObjects << object;
    }
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
