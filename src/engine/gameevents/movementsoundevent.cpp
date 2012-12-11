#include "movementsoundevent.h"

#include "character.h"
#include "point3d.h"
#include "room.h"
#include "util.h"
#include "vector3d.h"


#define super SoundEvent

MovementSoundEvent::MovementSoundEvent(Room *origin, double strength) :
    super(origin, strength),
    m_destination(nullptr) {
}

MovementSoundEvent::~MovementSoundEvent() {
}

void MovementSoundEvent::setDestination(Room *destination) {

    m_destination = destination;

    addVisit(destination, strengthForRoom(originRoom()));
}

void MovementSoundEvent::setMovement(const Vector3D &movement) {

    m_movement = movement;
}

void MovementSoundEvent::setDirection(const Vector3D &direction) {

    m_direction = direction;
}

QString MovementSoundEvent::descriptionForStrengthInRoom(double strength, Room *room) const {

    if (room == originRoom()) {
        return "You hear someone walking away.";
    } else if (room == m_destination) {
        return "You hear someone walking up to you.";
    }

    if (strength > 0.8) {
        QString direction;
        double angle = m_direction.angle(room->position() - originRoom()->position());
        if (angle < TAU / 4) {
            direction = "toward you";
        } else {
            direction = "away from you";
        }

        return QString("You hear someone walking %2.").arg(direction);
    } else {
        QString direction = Util::directionForVector(originRoom()->position() - room->position());
        if (strength > 0.6) {
            return QString("You hear %1 walking to the %2.").arg(direction);
        } else {
            return QString("You hear %1 moving to the %1.").arg(direction);
        }
    }
}
