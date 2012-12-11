#include "movementvisualevent.h"

#include "character.h"
#include "point3d.h"
#include "portal.h"
#include "room.h"
#include "util.h"
#include "vector3d.h"


#define super VisualEvent

MovementVisualEvent::MovementVisualEvent(Room *origin, double strength) :
    super(origin, strength),
    m_destination(nullptr) {
}

MovementVisualEvent::~MovementVisualEvent() {
}

void MovementVisualEvent::setDestination(Room *destination) {

    m_destination = destination;

    addVisit(destination, strengthForRoom(originRoom()));
}

void MovementVisualEvent::setMovement(const Vector3D &movement) {

    m_movement = movement;
}

void MovementVisualEvent::setDirection(const Vector3D &direction) {

    m_direction = direction;
}

QString MovementVisualEvent::descriptionForStrengthInRoom(double strength, Room *room) const {

    if (room == originRoom()) {
        QString characterName = Util::capitalize(description());
        for (const GameObjectPtr &portalPtr : room->portals()) {
            Portal *portal = portalPtr.cast<Portal *>();
            if (portal->room() == m_destination || portal->room2() == m_destination) {
                QString exitName = portal->nameFromRoom(room);
                if (Util::isDirection(exitName)) {
                    return QString("%1 walks %2.").arg(characterName, exitName);
                } else {
                    if (portal->canOpenFromRoom(room)) {
                        return QString("%1 walks through the %2.").arg(characterName, exitName);
                    } else {
                        return QString("%1 walks to the %2.").arg(characterName, exitName);
                    }
                }
            }
        }

        return QString("%1 walks %2.").arg(characterName, Util::directionForVector(m_direction));
    } else if (room == m_destination) {
        return QString("%1 walks to you.").arg(Util::capitalize(description()));
    }

    QString direction;
    double angle = m_direction.angle(room->position() - originRoom()->position());
    if (angle < TAU / 8) {
        direction = "toward you";
    } else {
        direction = Util::directionForVector(m_direction);
    }

    return QString("You see %1 walking %2.").arg(strength > 0.9 ? description() :
                                                 strength > 0.8 ? distantDescription() :
                                                 veryDistantDescription(), direction);
}

bool MovementVisualEvent::isWithinSight(Room *targetRoom, Room *sourceRoom) {

    if (sourceRoom == originRoom() || sourceRoom == m_destination) {
        return true;
    }

    Vector3D sourceVector = (sourceRoom->position() - originRoom()->position()).normalized();
    Vector3D targetVector = (targetRoom->position() - sourceRoom->position()).normalized();
    if (sourceVector == targetVector) {
        return true;
    }

    if (sourceRoom->flags() & RoomFlags::NoCeiling) {
        if (targetVector.z >= sourceVector.z) {
            return true;
        }
    }
    if (sourceRoom->flags() & RoomFlags::NoFloor) {
        if (targetVector.z <= sourceVector.z) {
            return true;
        }
    }

    sourceVector = (sourceRoom->position() - m_destination->position()).normalized();
    if (sourceVector == targetVector) {
        return true;
    }

    if (sourceRoom->flags() & RoomFlags::NoCeiling) {
        if (targetVector.z >= sourceVector.z) {
            return true;
        }
    }
    if (sourceRoom->flags() & RoomFlags::NoFloor) {
        if (targetVector.z <= sourceVector.z) {
            return true;
        }
    }

    return false;
}
