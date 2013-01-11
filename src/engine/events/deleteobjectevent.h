#ifndef DELETEOBJECTEVENT_H
#define DELETEOBJECTEVENT_H

#include "event.h"


class GameObject;

class DeleteObjectEvent : public Event {

    public:
        DeleteObjectEvent(uint objectId);
        virtual ~DeleteObjectEvent();

        virtual void process();

        virtual QString toString() const;

    private:
        uint m_objectId;
};

#endif // DELETEOBJECTEVENT_H
