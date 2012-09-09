#include "logthread.h"

#include "logmessage.h"


LogThread::LogThread() :
    QThread(),
    m_quit(false) {
}

LogThread::~LogThread() {
}

void LogThread::enqueueMessage(LogMessage *message) {

    m_mutex.lock();
    m_messageQueue.enqueue(message);
    m_mutex.unlock();

    m_waitCondition.wakeAll();
}

void LogThread::terminate() {

    m_quit = true;
    m_waitCondition.wakeAll();
}

void LogThread::run() {

    while (!m_quit) {
        m_mutex.lock();
        m_waitCondition.wait(&m_mutex);

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
