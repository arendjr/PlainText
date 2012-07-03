#ifndef GAMETHREAD_H
#define GAMETHREAD_H

#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>


class Event;
class Realm;

class GameThread : public QThread {

    public:
        GameThread(Realm *realm);
        virtual ~GameThread();

        void enqueueEvent(Event *event);

        void lock();
        void unlock();

        void terminate();

    protected:
        virtual void run();

    private:
        QWaitCondition m_waitCondition;
        QMutex m_mutex;
        volatile bool m_quit;

        Realm *m_realm;

        QQueue<Event *> m_eventQueue;

        void processEvent(Event *event);
};

#endif // GAMETHREAD_H
