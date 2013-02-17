#include "logthread.h"

#include "logmessage.h"
#include "logutil.h"


static const int MAX_QUEUE_SIZE = 10000;
static const int NUM_MESSAGES_TO_DROP = 100;


LogThread::LogThread() :
    QThread(),
    m_quit(false),
    m_numMessagesToDrop(0),
    m_numDroppedMessages(0) {
}

LogThread::~LogThread() {
}

void LogThread::enqueueMessage(LogMessage *message) {

    {
        QMutexLocker locker(&m_mutex);

        if (m_numMessagesToDrop > 0) {
            m_numDroppedMessages++;
            if (m_numDroppedMessages >= m_numMessagesToDrop) {
                if (m_messageQueue.size() >= MAX_QUEUE_SIZE) {
                    if (m_numMessagesToDrop >= MAX_QUEUE_SIZE) {
                        LogUtil::setLoggingEnabled(false);
                        LogUtil::logInfo("Log queue still full after dropping %1 messages. "
                                         "Logging system disabled.",
                                         QString::number(m_numDroppedMessages));
                    } else {
                        m_numMessagesToDrop *= 2;
                    }
                } else {
                    int numDroppedMessages = m_numDroppedMessages;
                    m_numMessagesToDrop = 0;
                    m_numDroppedMessages = 0;
                    locker.unlock();

                    LogUtil::logError("%1 messages have been dropped.",
                                      QString::number(numDroppedMessages));
                    LogUtil::logInfo("Logging resumes.");
                }
            }

            delete message;
            return;
        }

        if (m_messageQueue.size() >= MAX_QUEUE_SIZE) {
            m_numMessagesToDrop = NUM_MESSAGES_TO_DROP;
            m_numDroppedMessages = 1;

            LogUtil::logInfo("Log queue flooding detected. Log messages will be dropped.");

            delete message;
            return;
        }

        m_messageQueue.enqueue(message);
    }

    m_waitCondition.wakeAll();
}

void LogThread::terminate() {

    m_quit = true;
    m_waitCondition.wakeAll();
}

void LogThread::run() {

    while (!m_quit) {
        m_mutex.lock();

        if (m_messageQueue.isEmpty()) {
            m_waitCondition.wait(&m_mutex);
        }

        while (!m_quit && !m_messageQueue.isEmpty()) {
            LogMessage *message = m_messageQueue.dequeue();
            m_mutex.unlock();

            logMessage(message);

            m_mutex.lock();
        }

        m_mutex.unlock();
    }

    while (!m_messageQueue.isEmpty()) {
        LogMessage *message = m_messageQueue.dequeue();
        logMessage(message);
    }
}

void LogThread::logMessage(LogMessage *message) {

    message->log();
    delete message;
}
