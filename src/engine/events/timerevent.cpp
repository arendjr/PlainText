#include "timerevent.h"

#include "gameobject.h"


TimerEvent::TimerEvent(GameObject *object, int timerId) :
    Event(),
    m_object(object),
    m_timerId(timerId) {
}

TimerEvent::~TimerEvent() {
}

void TimerEvent::process() {

    m_object->invokeTimer(m_timerId);
}

QString TimerEvent::toString() const {

    return QString("Timer #%1 on object %2:%3")
           .arg(m_timerId).arg(m_object->objectType().toString()).arg(m_object->id());
}
