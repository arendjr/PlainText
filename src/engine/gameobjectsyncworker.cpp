#include "gameobjectsyncworker.h"

#include "gameobject.h"


GameObjectSyncWorker::GameObjectSyncWorker() :
    QObject() {

    connect(this, SIGNAL(objectEnqueued()), SLOT(onObjectEnqueued()), Qt::QueuedConnection);
}

void GameObjectSyncWorker::queueObject(const GameObject *object) {

    GameObject *copy = GameObject::createCopy(object);

    m_queueMutex.lock();
    m_objectQueue.enqueue(copy);
    m_queueMutex.unlock();

    emit objectEnqueued();
}

void GameObjectSyncWorker::syncAll() {

    while (!m_objectQueue.isEmpty()) {
        GameObject *object = m_objectQueue.dequeue();
        syncObject(object);
    }
}

void GameObjectSyncWorker::syncObject(GameObject *object) {

    object->save();

    delete object;
}

void GameObjectSyncWorker::onObjectEnqueued() {

    m_queueMutex.lock();
    GameObject *object = m_objectQueue.dequeue();
    m_queueMutex.unlock();

    syncObject(object);
}
