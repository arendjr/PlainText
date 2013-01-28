#ifndef FLOODEVENT_H
#define FLOODEVENT_H

#include "gameevent.h"


class FloodEvent : public GameEvent {

    Q_OBJECT

    public:
        FloodEvent(Room *origin, double strength);
        virtual ~FloodEvent();

        Q_INVOKABLE virtual QString descriptionForStrengthAndCharacterInRoom(double strength, Room *room) const;

    protected:
        virtual void visitRoom(Room *room, double strength);
};

#endif // FLOODEVENT_H
