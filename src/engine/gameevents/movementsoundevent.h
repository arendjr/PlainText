#ifndef MOVEMENTSOUNDEVENT_H
#define MOVEMENTSOUNDEVENT_H

#include "soundevent.h"

#include "vector3d.h"


class Character;

class MovementSoundEvent : public SoundEvent {

    Q_OBJECT

    public:
        MovementSoundEvent(Room *origin, double strength);
        virtual ~MovementSoundEvent();

        void setDestination(Room *destination);

        void setMovement(const Vector3D &movement);
        void setDirection(const Vector3D &direction);

        void setVerb(const QString &simplePresent, const QString &continuous);

        Q_INVOKABLE virtual QString descriptionForStrengthInRoom(double strength, Room *room) const;

    private:
        Room *m_destination;

        Vector3D m_movement;
        Vector3D m_direction;

        QString m_simplePresent;
        QString m_continuous;
};

#endif // MOVEMENTSOUNDEVENT_H
