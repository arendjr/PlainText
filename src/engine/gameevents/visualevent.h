#ifndef VISUALEVENT_H
#define VISUALEVENT_H

#include "gameevent.h"


class VisualEvent : public GameEvent {

    Q_OBJECT

    public:
        VisualEvent(Room *origin, double strength);
        VisualEvent(GameEventType eventType, Room *origin, double strength);
        virtual ~VisualEvent();

    protected:
        virtual void visitRoom(Room *room, double strength);

        virtual bool isWithinSight(Room *targetRoom, Room *sourceRoom);
};

#endif // VISUALEVENT_H
