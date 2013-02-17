#include "gameobjectsyncthread.h"

#include "gameexception.h"
#include "gameobject.h"
#include "logutil.h"


GameObjectSyncThread::GameObjectSyncThread() :
    QThread(),
    m_quit(false) {
}

GameObjectSyncThread::~GameObjectSyncThread() {
}

void GameObjectSyncThread::enqueueObject(GameObject *object) {

    GameObject *copy = GameObject::createCopy(object);

    m_mutex.lock();
    m_objectQueue.enqueue(copy);
    m_mutex.unlock();

    m_waitCondition.wakeAll();
}

void GameObjectSyncThread::terminate() {

    m_quit = true;
    m_waitCondition.wakeAll();
}

void GameObjectSyncThread::run() {

    while (!m_quit) {
        m_mutex.lock();

        if (m_objectQueue.isEmpty()) {
            m_waitCondition.wait(&m_mutex);
        }

        while (!m_quit && !m_objectQueue.isEmpty()) {
            GameObject *object = m_objectQueue.dequeue();
            m_mutex.unlock();

            syncObject(object);

            m_mutex.lock();
        }

        m_mutex.unlock();
    }

    while (!m_objectQueue.isEmpty()) {
        GameObject *object = m_objectQueue.dequeue();
        syncObject(object);
    }

    LogUtil::logInfo("All objects synced. Quit.");
}

void GameObjectSyncThread::syncObject(GameObject *object) {

    try {
        bool result = object->save();

        if (!result) {
            LogUtil::logError("Error while syncing object: %1:%2",
                              object->objectType().toString(), QString::number(object->id()));
        }
    } catch (const GameException &exception) {
        LogUtil::logError("Game Exception: %1\n"
                          "While syncing object: %2:%3", exception.what(),
                          object->objectType().toString(), QString::number(object->id()));
    } catch (...) {
        LogUtil::logError("Unknown exception while syncing object: %1:%2",
                          object->objectType().toString(), QString::number(object->id()));
    }

    delete object;
}
