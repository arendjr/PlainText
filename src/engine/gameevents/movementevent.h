#ifndef MOVEMENTEVENT_H
#define MOVEMENTEVENT_H

#include "visualevent.h"


class MovementEvent : public VisualEvent {

    Q_OBJECT

    public:
        MovementEvent(Room *origin, double strength);
        virtual ~MovementEvent();

        void setDestination(Room *destination);

    protected:
        virtual void visitRoom(Room *room, double strength);

    private:
        Room *m_destination;
};

#endif // MOVEMENTEVENT_H
