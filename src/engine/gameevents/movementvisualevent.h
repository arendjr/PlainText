#ifndef MOVEMENTVISUALEVENT_H
#define MOVEMENTVISUALEVENT_H

#include "gameobjectptr.h"
#include "room.h"
#include "vector3d.h"
#include "visualevent.h"


class MovementVisualEvent : public VisualEvent {

    Q_OBJECT

    public:
        MovementVisualEvent(Room *origin, double strength);
        virtual ~MovementVisualEvent();

        GameObjectPtr destination() { return GameObjectPtr(m_destination); }
        void setDestination(const GameObjectPtr &destination);
        Q_PROPERTY(GameObjectPtr destination READ destination WRITE setDestination)

        const Vector3D &movement() const { return m_movement; }
        void setMovement(const Vector3D &movement);
        Q_PROPERTY(Vector3D movement READ movement WRITE setMovement)

        const Vector3D &direction() const { return m_direction; }
        void setDirection(const Vector3D &direction);
        Q_PROPERTY(Vector3D direction READ direction WRITE setDirection)

        const QString &simplePresent() const { return m_simplePresent; }
        Q_PROPERTY(QString simplePresent READ simplePresent)

        const QString &continuous() const { return m_continuous; }
        Q_PROPERTY(QString continuous READ continuous)

        Q_INVOKABLE void setVerb(const QString &simplePresent, const QString &continuous);

        Q_INVOKABLE virtual QString descriptionForStrengthAndCharacterInRoom(double strength,
                                                                             Character *character,
                                                                             Room *room) const;

    protected:
        virtual bool isWithinSight(Room *targetRoom, Room *sourceRoom);

    private:
        Room *m_destination;

        Vector3D m_movement;
        Vector3D m_direction;

        QString m_simplePresent;
        QString m_continuous;
};

#endif // MOVEMENTVISUALEVENT_H
