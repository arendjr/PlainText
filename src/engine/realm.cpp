#include "realm.h"

#include <cstring>

#include <QDebug>
#include <QDir>
#include <QTimerEvent>

#include "gameexception.h"
#include "gameobjectsyncthread.h"
#include "player.h"
#include "scriptengine.h"


Realm *Realm::s_instance = 0;


void Realm::instantiate() {

    Q_ASSERT(s_instance == 0);
    new Realm();
}

void Realm::destroy() {

    delete s_instance;
    s_instance = 0;
}

Realm::Realm(Options options) :
    GameObject("realm", 0, options),
    m_initialized(false),
    m_nextId(1),
    m_syncThread(0) {

    if (options & Copy) {
        return;
    }

    s_instance = this;

    load(saveObjectPath(objectType(), id()));

    QDir dir(saveDirPath());
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        if (fileName.startsWith("realm.")) {
            continue;
        }

        try {
            GameObject::createFromFile(dir.path() + "/" + fileName);
        } catch (const GameException &exception) {
            qWarning() << "Error loading game object from " << fileName << ":" << exception.what();
        }
    }

    foreach (GameObject *gameObject, m_objectMap) {
        Q_ASSERT(gameObject);
        gameObject->resolvePointers();
    }

    m_syncThread = new GameObjectSyncThread(this);
    m_syncThread->start(QThread::LowestPriority);

    ScriptEngine::instance()->setGlobalObject("Realm", this);

    m_initialized = true;

    foreach (GameObject *gameObject, m_objectMap) {
        gameObject->init();
    }

    m_timeTimer = startTimer(150000);
}

Realm::~Realm() {

    ScriptEngine::instance()->unsetGlobalObject("Realm");

    m_syncThread->quit();

    foreach (GameObject *gameObject, m_objectMap) {
        Q_ASSERT(gameObject);
        delete gameObject;
    }

    m_syncThread->wait();
}

void Realm::registerObject(GameObject *gameObject) {

    Q_ASSERT(gameObject);

    uint id = gameObject->id();
    m_objectMap.insert(id, gameObject);

    if (strcmp(gameObject->objectType(), "race") == 0) {
        m_races.append(gameObject);
    } else if (strcmp(gameObject->objectType(), "class") == 0) {
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
    foreach (Player *player, m_playerMap) {
        players << player;
    }
    return players;
}

GameObjectPtrList Realm::onlinePlayers() const {

    GameObjectPtrList players;
    foreach (Player *player, m_playerMap) {
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

void Realm::syncObject(GameObject *gameObject) {

    if (!m_initialized) {
        return;
    }

    m_syncThread->queueObject(gameObject);
}

QString Realm::saveDirPath() {

    return QDir::homePath() + "/.mud";
}

QString Realm::saveObjectPath(const char *objectType, uint id) {

    return (saveDirPath() + "/%1.%2").arg(objectType).arg(id, 9, 10, QChar('0'));
}

void Realm::timerEvent(QTimerEvent *event) {

    int id = event->timerId();
    if (id == m_timeTimer) {
        m_dateTime = m_dateTime.addSecs(3600);

        emit hourPassed(m_dateTime);
        if (m_dateTime.time().hour() == 0) {
            emit dayPassed(m_dateTime);
        }
    } else {
        GameObject::timerEvent(event);
    }
}
