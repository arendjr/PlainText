#include "gamethread.h"

#include <QDateTime>

#include "event.h"
#include "gameexception.h"
#include "logutil.h"
#include "realm.h"
#include "timerevent.h"


GameThread::GameThread(Realm *realm) :
    QThread(),
    m_quit(false),
    m_realm(realm),
    m_nextTimerId(0) {
}

GameThread::~GameThread() {
}

void GameThread::enqueueEvent(Event *event) {

    m_mutex.lock();
    m_eventQueue.enqueue(event);
    m_mutex.unlock();

    m_waitCondition.wakeAll();
}

void GameThread::terminate() {

    m_quit = true;
    m_waitCondition.wakeAll();
}

int GameThread::startTimer(GameObject *object, int timeout) {

    m_nextTimerId++;
    if (m_nextTimerId < 0) {
        m_nextTimerId = 1;
    }

    Timer timer;
    timer.id = m_nextTimerId;
    timer.object = object;
    timer.timestamp = QDateTime::currentMSecsSinceEpoch() + timeout;
    timer.interval = 0;

    enqueueTimer(timer);

    return timer.id;
}

int GameThread::startInterval(GameObject *object, int interval) {

    m_nextTimerId++;
    if (m_nextTimerId < 0) {
        m_nextTimerId = 1;
    }

    Timer timer;
    timer.id = m_nextTimerId;
    timer.object = object;
    timer.timestamp = QDateTime::currentMSecsSinceEpoch() + interval;
    timer.interval = interval;

    enqueueTimer(timer);

    return timer.id;
}

void GameThread::stopTimer(int id) {

    dequeueTimer(id);
}

void GameThread::stopInterval(int id) {

    dequeueTimer(id);
}

void GameThread::run() {

    while (!m_quit) {
        m_mutex.lock();

        unsigned long msecs = msecsTillNextTimer();
        bool timeout = (msecs ? !m_waitCondition.wait(&m_mutex, msecs) : true);

        while (!m_quit && (!m_eventQueue.isEmpty() || timeout)) {
            Event *event;
            if (timeout) {
                event = takeFirstTimer();
                timeout = false;
            } else {
                event = m_eventQueue.dequeue();
            }

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
        LogUtil::logError("Game Exception: %1\n"
                          "While processing event: %2", exception.what(), event->toString());
    } catch (...) {
        LogUtil::logError("Unknown exception while processing event: %2", event->toString());
    }

    delete event;
}

unsigned long GameThread::msecsTillNextTimer() const {

    if (m_timers.isEmpty()) {
        return ULONG_MAX;
    }

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    return qMax(m_timers[0].timestamp, now) - now;
}

Event *GameThread::takeFirstTimer() {

    Q_ASSERT(!m_timers.isEmpty());

    Timer timer = m_timers.takeFirst();

    if (timer.interval) {
        timer.timestamp += timer.interval;
        enqueueTimer(timer);
    }

    return new TimerEvent(timer.object, timer.id);
}

void GameThread::enqueueTimer(const GameThread::Timer &timer) {

    if (m_timers.isEmpty()) {
        m_timers.append(timer);
    } else {
        auto iterator = qUpperBound(m_timers.begin(), m_timers.end(), timer);
        m_timers.insert(iterator, timer);
    }
}

void GameThread::dequeueTimer(int id) {

    for (int i = 0; i < m_timers.length(); i++) {
        if (m_timers[i].id == id) {
            m_timers.removeAt(i);
            return;
        }
    }
}

bool GameThread::Timer::operator<(const GameThread::Timer &timer) const {

    return timestamp < timer.timestamp;
}
