#ifndef GAMETHREAD_H
#define GAMETHREAD_H

#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>


class Event;
class GameObject;
class Realm;

class GameThread : public QThread {

    public:
        GameThread(Realm *realm);
        virtual ~GameThread();

        void enqueueEvent(Event *event);

        void terminate();

        int startTimer(GameObject *object, int timeout);
        int startInterval(GameObject *object, int interval);

        void stopTimer(int id);
        void stopInterval(int id);

    protected:
        virtual void run();

    private:
        QWaitCondition m_waitCondition;
        QMutex m_mutex;
        volatile bool m_quit;

        Realm *m_realm;

        QQueue<Event *> m_eventQueue;

        struct Timer {
            int id;
            qint64 timestamp;
            GameObject *object;
            int interval;

            bool operator<(const Timer &timer) const;
        };

        QList<Timer> m_timers;
        int m_nextTimerId;

        void processEvent(Event *event);

        unsigned long msecsTillNextTimer() const;
        Event *takeFirstTimer();
        void enqueueTimer(const Timer &timer);
        void dequeueTimer(int id);
};

#endif // GAMETHREAD_H
