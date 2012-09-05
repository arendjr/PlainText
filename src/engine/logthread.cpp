#include "logthread.h"


LogThread::LogThread() :
    QThread(),
    m_quit(false) {
}

LogThread::~LogThread() {
}

void LogThread::terminate() {

    m_quit = true;
    m_waitCondition.wakeAll();
}

void LogThread::run() {

    while (!m_quit) {
        m_mutex.lock();
        m_waitCondition.wait(&m_mutex);

        while (!m_quit && !m_eventQueue.isEmpty()) {
            LogEvent *event = m_eventQueue.dequeue();
            m_mutex.unlock();

            logEvent(event);

            m_mutex.lock();
        }

        m_mutex.unlock();
    }

    while (!m_eventQueue.isEmpty()) {
        LogEvent *event = m_eventQueue.dequeue();
        logEvent(event);
    }
}

void LogThread::logEvent(LogEvent *event) {

    delete event;
}
