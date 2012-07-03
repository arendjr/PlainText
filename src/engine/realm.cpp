#include "realm.h"

#include <cstring>

#include <QDebug>
#include <QDir>
#include <QTimerEvent>

#include "gameexception.h"
#include "gameobjectsyncthread.h"
#include "player.h"


static Realm *s_instance = nullptr;


Realm::Realm(Options options) :
    GameObject(this, "realm", 0, options),
    m_initialized(false),
    m_nextId(1),
    m_gameThread(this) {

    if (~options & Copy) {
        s_instance = this;
    }
}

Realm::~Realm() {

    m_gameThread.terminate();
    m_gameThread.wait();

    m_syncThread.terminate();

    for (GameObject *gameObject : m_objectMap) {
        Q_ASSERT(gameObject);
        delete gameObject;
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
        try {
            if (!fileName.startsWith("realm.")) {
                GameObject::createFromFile(this, dir.path() + "/" + fileName);
            }
        } catch (const GameException &exception) {
            qWarning() << "Error loading game object from " << fileName.toUtf8().constData()
                       << ": " << exception.what();
        }
    }

    for (GameObject *object : m_objectMap) {
        object->resolvePointers();
    }

    m_syncThread.start(QThread::LowestPriority);
    m_gameThread.start(QThread::HighestPriority);

    m_initialized = true;

    for (GameObject *object : m_objectMap) {
        try {
            object->init();
        } catch (const GameException &exception) {
            qWarning() << "Error initializing game object "
                       << object->objectType() << ":" << object->id() << ": " << exception.what();
        }
    }

    m_timeTimer = startTimer(150000);
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

void Realm::lock() {

    m_gameThread.lock();
}

void Realm::unlock() {

    m_gameThread.unlock();
}

void Realm::timerEvent(int timerId) {

    if (timerId == m_timeTimer) {
        m_dateTime = m_dateTime.addSecs(3600);

        emit hourPassed(m_dateTime);
        if (m_dateTime.time().hour() == 0) {
            emit dayPassed(m_dateTime);
        }
    } else {
        GameObject::timerEvent(timerId);
    }
}
