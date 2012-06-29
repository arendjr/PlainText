#ifndef GAMEOBJECTSYNCTHREAD_H
#define GAMEOBJECTSYNCTHREAD_H

#include <QThread>


class GameObject;
class GameObjectSyncWorker;

class GameObjectSyncThread : public QThread {

    Q_OBJECT

    public:
        explicit GameObjectSyncThread(QObject *parent = 0);
        virtual ~GameObjectSyncThread();

        void queueObject(const GameObject *object);

    protected:
        virtual void run();

    private:
        GameObjectSyncWorker *m_worker;
};

#endif // GAMEOBJECTSYNCTHREAD_H
