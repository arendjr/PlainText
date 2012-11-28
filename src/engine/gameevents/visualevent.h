#ifndef VISUALEVENT_H
#define VISUALEVENT_H

#include "gameevent.h"


class VisualEvent : public GameEvent {

    Q_OBJECT

    public:
        VisualEvent(Room *origin, double strength);
        virtual ~VisualEvent();

    protected:
        virtual void visitRoom(Room *room, double strength);
};

#endif // VISUALEVENT_H
