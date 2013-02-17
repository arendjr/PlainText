#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>


class LogMessage;

class LogThread : public QThread {

    Q_OBJECT

    public:
        LogThread();
        virtual ~LogThread();

        void enqueueMessage(LogMessage *message);

        void terminate();

    protected:
        virtual void run();

    private:
        QWaitCondition m_waitCondition;
        QMutex m_mutex;
        volatile bool m_quit;

        QQueue<LogMessage *> m_messageQueue;

        int m_numMessagesToDrop;
        int m_numDroppedMessages;

        void logMessage(LogMessage *message);
};

#endif // LOGTHREAD_H
