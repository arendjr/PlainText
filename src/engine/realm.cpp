#include "realm.h"

#include <cstring>

#include <QDir>

#include "player.h"


static Realm *s_instance = nullptr;


Realm::Realm(Options options) :
    GameObject(this, "realm", 0, options),
    m_initialized(false),
    m_nextId(1),
    m_timeIntervalId(0),
    m_gameThread(this),
    m_scriptEngine(nullptr) {

    if (~options & Copy) {
        s_instance = this;
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

    if (m_initialized) {
        for (GameObject *object : m_objectMap) {
            delete object;
        }
    }

    m_syncThread.wait();
}

Realm *Realm::instance() {

    return s_instance;
}

void Realm::init() {

    load(saveObjectPath(objectType(), id()));

    QDir dir(saveDirPath());
    for (const QString &fileName : dir.entryList(QDir::Files)) {
        if (!fileName.startsWith("realm.")) {
            GameObject::createFromFile(this, dir.path() + "/" + fileName);
        }
    }

    for (GameObject *object : m_objectMap) {
        object->resolvePointers();
    }

    m_syncThread.start(QThread::LowestPriority);
    m_gameThread.start(QThread::HighestPriority);

    m_initialized = true;

    for (GameObject *object : m_objectMap) {
        object->init();
    }

    m_timeIntervalId = startInterval(this, 150000);
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

GameObject *Realm::getObject(const char *objectType, uint id) const {

    if (m_objectMap.contains(id)) {
        GameObject *object = m_objectMap[id];
        Q_ASSERT(object);
        if (!objectType || strcmp(object->objectType(), objectType) == 0) {
            return object;
        }
    }

    return 0;
}

GameObject *Realm::getObject(const QString &objectType, uint id) const {

    return getObject(objectType.toAscii().constData(), id);
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
        GameObject::invokeTimer(timerId);
    }
}
