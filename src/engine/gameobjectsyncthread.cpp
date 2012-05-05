#include "gameobjectsyncthread.h"

#include "gameobject.h"


GameObjectSyncThread::GameObjectSyncThread(QObject *parent) :
    QThread(parent) {

    connect(this, SIGNAL(objectEnqueued()), SLOT(onObjectEnqueued()), Qt::QueuedConnection);
}

void GameObjectSyncThread::queueObject(const GameObject *object) {

    GameObject *copy = GameObject::createCopy(object);

    m_queueMutex.lock();
    m_objectQueue.enqueue(copy);
    m_queueMutex.unlock();

    emit objectEnqueued();
}

void GameObjectSyncThread::run() {

    exec();

    while (!m_objectQueue.isEmpty()) {
        GameObject *object = m_objectQueue.dequeue();
        syncObject(object);
    }
}

void GameObjectSyncThread::syncObject(GameObject *object) {

    object->save();

    delete object;
}

void GameObjectSyncThread::onObjectEnqueued() {

    m_queueMutex.lock();
    GameObject *object = m_objectQueue.dequeue();
    m_queueMutex.unlock();

    syncObject(object);
}
