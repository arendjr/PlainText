#ifndef GAMEOBJECTSYNCTHREAD_H
#define GAMEOBJECTSYNCTHREAD_H

#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>


class GameObject;

class GameObjectSyncThread : public QThread {

    Q_OBJECT

    public:
        GameObjectSyncThread();
        virtual ~GameObjectSyncThread();

        void enqueueObject(GameObject *object);

        void terminate();

    protected:
        virtual void run();

    private:
        QWaitCondition m_waitCondition;
        QMutex m_mutex;
        volatile bool m_quit;

        QQueue<GameObject *> m_objectQueue;

        void syncObject(GameObject *object);
};

#endif // GAMEOBJECTSYNCTHREAD_H
