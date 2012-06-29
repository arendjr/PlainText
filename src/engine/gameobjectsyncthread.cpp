#include "gameobjectsyncthread.h"

#include "gameobjectsyncworker.h"


GameObjectSyncThread::GameObjectSyncThread(QObject *parent) :
    QThread(parent),
    m_worker(0) {
}

GameObjectSyncThread::~GameObjectSyncThread() {

    delete m_worker;
}

void GameObjectSyncThread::queueObject(const GameObject *object) {

    Q_ASSERT(m_worker);
    m_worker->queueObject(object);
}

void GameObjectSyncThread::run() {

    m_worker = new GameObjectSyncWorker();

    exec();

    m_worker->syncAll();
}
