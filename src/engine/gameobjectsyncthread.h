#ifndef GAMEOBJECTSYNCTHREAD_H
#define GAMEOBJECTSYNCTHREAD_H

#include <QMutex>
#include <QQueue>
#include <QThread>


class GameObject;

class GameObjectSyncThread : public QThread {

    Q_OBJECT

    public:
        explicit GameObjectSyncThread(QObject *parent = 0);

        void queueObject(const GameObject *object);

    signals:
        void objectEnqueued();

    protected:
        virtual void run();

        void syncObject(GameObject *object);

    protected slots:
        void onObjectEnqueued();

    private:
        QQueue<GameObject *> m_objectQueue;
        QMutex m_queueMutex;
};

#endif // GAMEOBJECTSYNCTHREAD_H
