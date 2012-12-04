#ifndef MOVEMENTEVENT_H
#define MOVEMENTEVENT_H

#include "visualevent.h"

#include "vector3d.h"


class Character;

class MovementEvent : public VisualEvent {

    Q_OBJECT

    public:
        MovementEvent(Character *character, Room *origin, double strength);
        virtual ~MovementEvent();

        void setDestination(Room *destination);

        void setMovement(const Vector3D &movement);
        void setDirection(const Vector3D &direction);

        Q_INVOKABLE virtual QString descriptionForStrengthInRoom(double strength, Room *room) const;

    protected:
        virtual bool isWithinSight(Room *targetRoom, Room *sourceRoom);

    private:
        Character *m_character;
        Room *m_destination;

        Vector3D m_movement;
        Vector3D m_direction;
};

#endif // MOVEMENTEVENT_H
