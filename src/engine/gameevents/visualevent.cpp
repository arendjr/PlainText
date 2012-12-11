#include "visualevent.h"

#include "character.h"
#include "portal.h"
#include "room.h"
#include "util.h"
#include "vector3d.h"


#define super GameEvent

VisualEvent::VisualEvent(Room *origin, double strength) :
    super(GameEventType::VisualEvent, origin, strength) {
}

VisualEvent::~VisualEvent() {
}

void VisualEvent::visitRoom(Room *room, double strength) {

    strength *= room->eventMultipliers()[GameEventType::VisualEvent];

    if (strength >= 0.1) {
        QString message = descriptionForStrengthInRoom(strength, room);

        for (const GameObjectPtr &characterPtr : room->characters()) {
            if (excludedCharacters().contains(characterPtr)) {
                continue;
            }

            // make sure the character is actually looking in the direction of the event
            Character *character = characterPtr.cast<Character *>();
            Vector3D eventDirection = originRoom()->position() - room->position();
            if (!eventDirection.isNull()) {
                double angle = character->direction().angle(eventDirection);
                if (angle > TAU / 8) {
                    continue;
                }
            }

            if (character->isPlayer()) {
                character->send(message);
            } else {
                character->invokeTrigger("onvisual", message);
            }
            addAffectedCharacter(character);
        }

        for (const GameObjectPtr &portalPtr : room->portals()) {
            Portal *portal = portalPtr.unsafeCast<Portal *>();
            Room *room1 = portal->room().unsafeCast<Room *>();
            Room *room2 = portal->room2().unsafeCast<Room *>();
            Room *oppositeRoom = (room == room1 ? room2 : room1);
            if (hasBeenVisited(oppositeRoom)) {
                continue;
            }

            if (!isWithinSight(oppositeRoom, room)) {
                continue;
            }

            double multiplier;
            if (portal->isOpen()) {
                if (!portal->canSeeThroughIfOpen()) {
                    continue;
                }
                multiplier = 1.0;
            } else {
                if (!portal->canSeeThrough()) {
                    continue;
                }
                multiplier = portal->eventMultipliers()[GameEventType::VisualEvent];
            }

            Vector3D vector = oppositeRoom->position() - room->position();
            multiplier *= qMax(1.0 - 0.0005 * vector.length(), 0.0);

            double propagatedStrength = strength * multiplier;
            if (propagatedStrength >= 0.1) {
                addVisit(oppositeRoom, propagatedStrength);
            }
        }
    }
}

bool VisualEvent::isWithinSight(Room *targetRoom, Room *sourceRoom) {

    if (sourceRoom == originRoom()) {
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

    return false;
}
