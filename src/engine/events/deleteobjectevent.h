#ifndef DELETEOBJECTEVENT_H
#define DELETEOBJECTEVENT_H

#include "event.h"


class GameObject;

class DeleteObjectEvent : public Event {

    public:
        DeleteObjectEvent(GameObject *object);
        virtual ~DeleteObjectEvent();

        virtual void process();

        virtual QString toString() const;

    private:
        GameObject *m_object;
};

#endif // DELETEOBJECTEVENT_H
