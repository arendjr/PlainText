#ifndef GAMEOBJECTSYNCWORKER_H
#define GAMEOBJECTSYNCWORKER_H

#include <QMutex>
#include <QObject>
#include <QQueue>


class GameObject;

class GameObjectSyncWorker : public QObject {

    Q_OBJECT

    public:
        GameObjectSyncWorker();

        void queueObject(const GameObject *object);

        void syncAll();

    signals:
        void objectEnqueued();

    protected:
        void syncObject(GameObject *object);

    protected slots:
        void onObjectEnqueued();

    private:
        QQueue<GameObject *> m_objectQueue;
        QMutex m_queueMutex;
};

#endif // GAMEOBJECTSYNCWORKER_H
