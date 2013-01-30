#include "movementsoundevent.h"

#include <cmath>

#include <QDebug>

#include "character.h"
#include "gameexception.h"
#include "point3d.h"
#include "room.h"
#include "util.h"
#include "vector3d.h"


#define super SoundEvent

MovementSoundEvent::MovementSoundEvent(Room *origin, double strength) :
    super(GameEventType::MovementSound, origin, strength),
    m_destination(nullptr),
    m_movement(0, 0, 0),
    m_direction(0, 0, 0) {
}

MovementSoundEvent::~MovementSoundEvent() {
}

void MovementSoundEvent::setDestination(const GameObjectPtr &destination) {

    try {
        m_destination = destination.cast<Room *>();

        addVisit(m_destination, strengthForRoom(originRoom()));
    } catch (GameException &exception) {
        qDebug() << "Exception in MovementSoundEvent::setDestination(): " << exception.what();
    }
}

void MovementSoundEvent::setMovement(const Vector3D &movement) {

    m_movement = movement;
}

void MovementSoundEvent::setDirection(const Vector3D &direction) {

    m_direction = direction;
}

void MovementSoundEvent::setVerb(const QString &simplePresent, const QString &continuous) {

    m_simplePresent = simplePresent;
    if (continuous.contains(' ')) {
        m_helperVerb = continuous.split(' ')[0];
        m_continuous = continuous.split(' ')[1];
    } else {
        m_continuous = continuous;
    }
}

QString MovementSoundEvent::descriptionForStrengthAndCharacterInRoom(double strength,
                                                                     Character *character,
                                                                     Room *room) const {

    if (room == originRoom()) {
        return QString("You hear %1 %2 away.").arg(description(), m_continuous);
    } else if (room == m_destination) {
        Vector3D vector = room->position() - originRoom()->position();
        double angle = atan2(vector.y, vector.x) -
                       atan2(character->direction().y, character->direction().x);
        if (angle < -TAU / 2) {
            angle += TAU;
        } else if (angle > TAU / 2) {
            angle -= TAU;
        }

        QString direction;
        if (angle > TAU * 3 / 8 || angle < -TAU * 3 / 8) {
            // from the front
        } else if (angle < TAU / 8 && angle > -TAU / 8) {
            direction = " from behind";
        } else {
            if (angle > 0) {
                direction = " from the left";
            } else {
                direction = " from the right";
            }
        }
        return QString("You hear %1 %2 up to you%3.").arg(description(), m_continuous, direction);
    }

    QString description = (strength > 0.6 ? distantDescription() : veryDistantDescription());

    QString direction;
    if (strength > 0.8) {
        double angle = m_direction.angle(room->position() - originRoom()->position());
        if (angle < TAU / 4) {
            direction = "toward you";
        } else {
            direction = "away from you";
        }
    } else {
        direction = "to the " +
                    Util::directionForVector(originRoom()->position() - room->position());
    }

    return QString("You hear %1 %2 %3.").arg(description, m_continuous, direction);
}
