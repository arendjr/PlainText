#include "gameobjectsyncthread.h"

#include <QDebug>

#include "gameexception.h"
#include "gameobject.h"


GameObjectSyncThread::GameObjectSyncThread() :
    QThread(),
    m_quit(false) {
}

GameObjectSyncThread::~GameObjectSyncThread() {
}

void GameObjectSyncThread::enqueueObject(const GameObject *object) {

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
        m_waitCondition.wait(&m_mutex);

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

    qDebug() << "All objects synced. Quit.";
}

void GameObjectSyncThread::syncObject(GameObject *object) {

    try {
        object->save();
    } catch (const GameException &exception) {
        qWarning() << "Game Exception: " << exception.what() << endl
                   << QString("While syncing object: %1:%2")
                      .arg(object->objectType().toString(), object->id());
    } catch (...) {
        qWarning() << "Unknown exception." << endl
                   << QString("While syncing object: %1:%2")
                      .arg(object->objectType().toString(), object->id());
    }

    delete object;
}
