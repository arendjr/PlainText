#ifndef TIMEREVENT_H
#define TIMEREVENT_H

#include "event.h"


class GameObject;

class TimerEvent : public Event {

    public:
        TimerEvent(GameObject *object, int timerId);
        virtual ~TimerEvent();

        virtual void process();

        virtual QString toString() const;

    private:
        GameObject *m_object;
        int m_timerId;
};

#endif // TIMEDEVENT_H
