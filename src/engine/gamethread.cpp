#include "gamethread.h"

#include <QDebug>

#include "event.h"
#include "gameexception.h"
#include "realm.h"


GameThread::GameThread(Realm *realm) :
    QThread(),
    m_quit(false),
    m_realm(realm) {
}

GameThread::~GameThread() {
}

void GameThread::enqueueEvent(Event *event) {

    m_mutex.lock();
    m_eventQueue.enqueue(event);
    m_mutex.unlock();

    m_waitCondition.wakeAll();
}

void GameThread::lock() {

    m_mutex.lock();
}

void GameThread::unlock() {

    m_mutex.unlock();
}

void GameThread::terminate() {

    m_quit = false;
    m_waitCondition.wakeAll();
}

void GameThread::run() {

    while (!m_quit) {
        m_mutex.lock();
        m_waitCondition.wait(&m_mutex);

        while (!m_quit && !m_eventQueue.isEmpty()) {
            Event *event = m_eventQueue.dequeue();
            m_mutex.unlock();

            processEvent(event);

            m_mutex.lock();
        }

        m_mutex.unlock();
    }

    while (!m_eventQueue.isEmpty()) {
        Event *event = m_eventQueue.dequeue();
        processEvent(event);
    }
}

void GameThread::processEvent(Event *event) {

    try {
        event->process();

        m_realm->enqueueModifiedObjects();
    } catch (const GameException &exception) {
        qWarning() << "Game Exception: " << exception.what() << endl
                   << "While processing event: " << event->toString().toUtf8().constData();
    } catch (...) {
        qWarning() << "Unknown exception." << endl
                   << "While processing event: " << event->toString().toUtf8().constData();
    }

    delete event;
}
