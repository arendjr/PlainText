#include "visualevent.h"

#include "character.h"
#include "portal.h"
#include "room.h"
#include "util.h"
#include "vector3d.h"


#define super GameEvent

VisualEvent::VisualEvent(Room *origin, double strength) :
    VisualEvent(GameEventType::Visual, origin, strength) {
}

VisualEvent::VisualEvent(GameEventType eventType, Room *origin, double strength) :
    super(eventType, origin, strength) {
}

VisualEvent::~VisualEvent() {
}

void VisualEvent::visitRoom(Room *room, double strength) {

    strength *= room->eventMultipliers()[GameEventType::Visual];

    if (strength >= 0.1) {
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

            QString message = descriptionForStrengthAndCharacterInRoom(strength, character, room);
            if (character->isPlayer()) {
                character->send(message);
            } else {
                character->invokeTrigger("onvisual", message);
            }

            addAffectedCharacter(characterPtr);
        }

        for (const GameObjectPtr &portalPtr : room->portals()) {
            Portal *portal = portalPtr.unsafeCast<Portal *>();
            Room *room1 = portal->room().unsafeCast<Room *>();
            Room *room2 = portal->room2().unsafeCast<Room *>();
            Room *oppositeRoom = (room == room1 ? room2 : room1);
            if (hasBeenVisited(oppositeRoom)) {
                continue;
            }

            if (!portal->canSeeThrough() || !isWithinSight(oppositeRoom, room)) {
                continue;
            }

            double propagatedStrength = strength * portal->eventMultiplier(GameEventType::Visual);
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

    if (~sourceRoom->flags() & RoomFlags::HasWalls) {
        if (targetVector.z == sourceVector.z) {
            return true;
        }
    } else {
        if (targetVector.x != sourceVector.x || targetVector.y != sourceVector.y) {
            return false;
        }
    }

    return ((~sourceRoom->flags() & RoomFlags::HasCeiling && targetVector.z >= sourceVector.z) ||
            (~sourceRoom->flags() & RoomFlags::HasFloor && targetVector.z <= sourceVector.z));
}
