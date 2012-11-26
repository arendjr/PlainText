#include "visualevent.h"

#include "portal.h"
#include "room.h"
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

        for (const GameObjectPtr &character : room->characters()) {
            if (excludedCharacters().contains(character)) {
                continue;
            }

            if (character->isPlayer()) {
                character->send(message);
            } else {
                character->invokeTrigger("onvisual", message);
            }
            addAffectedCharacter(character);
        }

        for (const GameObjectPtr &portalPtr : room->portals()) {
            Portal *portal = portalPtr.cast<Portal *>();
            Room *room1 = portal->room().cast<Room *>();
            Room *room2 = portal->room2().cast<Room *>();
            Room *oppositeRoom = (room == room1 ? room2 : room1);
            if (hasBeenVisited(oppositeRoom)) {
                continue;
            }

            Vector3D vector = oppositeRoom->position() - room->position();

            bool inSight = true;

            if (room != originRoom()) {
                Vector3D sourceVector = (room->position() - originRoom()->position()).normalized();
                Vector3D targetVector = vector.normalized();
                if (sourceVector != targetVector) {
                    inSight = false;

                    if (room->flags() & RoomFlags::NoCeiling) {
                        if (targetVector.z >= sourceVector.z) {
                            inSight = true;
                        }
                    }
                    if (room->flags() & RoomFlags::NoFloor) {
                        if (targetVector.z <= sourceVector.z) {
                            inSight = true;
                        }
                    }
                }
            }

            if (!inSight) {
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

            multiplier *= qMax(1.0 - 0.0005 * vector.length(), 0.0);

            double propagatedStrength = strength * multiplier;
            if (propagatedStrength >= 0.1) {
                addVisit(oppositeRoom, propagatedStrength);
            }
        }
    }
}
