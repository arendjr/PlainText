#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>


class LogEvent;

class LogThread : public QThread {

    Q_OBJECT

    public:
        LogThread();
        virtual ~LogThread();

        void terminate();

    protected:
        virtual void run();

    private:
        QWaitCondition m_waitCondition;
        QMutex m_mutex;
        volatile bool m_quit;

        QQueue<LogEvent *> m_eventQueue;

        void logEvent(LogEvent *event);
};

#endif // LOGTHREAD_H
