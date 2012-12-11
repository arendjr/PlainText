#ifndef MOVEMENTVISUALEVENT_H
#define MOVEMENTVISUALEVENT_H

#include "visualevent.h"

#include "vector3d.h"


class Character;

class MovementVisualEvent : public VisualEvent {

    Q_OBJECT

    public:
        MovementVisualEvent(Room *origin, double strength);
        virtual ~MovementVisualEvent();

        void setDestination(Room *destination);

        void setMovement(const Vector3D &movement);
        void setDirection(const Vector3D &direction);

        Q_INVOKABLE virtual QString descriptionForStrengthInRoom(double strength, Room *room) const;

    protected:
        virtual bool isWithinSight(Room *targetRoom, Room *sourceRoom);

    private:
        Room *m_destination;

        Vector3D m_movement;
        Vector3D m_direction;
};

#endif // MOVEMENTVISUALEVENT_H
