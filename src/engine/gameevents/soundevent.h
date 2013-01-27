#ifndef SOUNDEVENT_H
#define SOUNDEVENT_H

#include "gameevent.h"


class SoundEvent : public GameEvent {

    Q_OBJECT

    public:
        SoundEvent(Room *origin, double strength);
        SoundEvent(GameEventType eventType, Room *origin, double strength);
        virtual ~SoundEvent();

    protected:
        virtual void visitRoom(Room *room, double strength);
};

#endif // SOUNDEVENT_H
