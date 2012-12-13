#ifndef AREAEVENT_H
#define AREAEVENT_H

#include "gameevent.h"


class AreaEvent : public GameEvent {

    Q_OBJECT

    public:
        AreaEvent(Room *origin, double strength);
        virtual ~AreaEvent();

    protected:
        virtual void visitRoom(Room *room, double strength);
};

#endif // AREAEVENT_H
