#include "soundevent.h"

#include "portal.h"
#include "room.h"


#define super GameEvent

SoundEvent::SoundEvent(Room *origin, double strength) :
    super(GameEventType::SoundEvent, origin, strength) {
}

SoundEvent::~SoundEvent() {
}

void SoundEvent::visitRoom(Room *room, double strength) {

    strength *= room->eventMultipliers()[GameEventType::SoundEvent];

    if (strength >= 0.1) {
        QString message = descriptionForStrengthInRoom(strength, room);

        for (const GameObjectPtr &character : room->characters()) {
            if (excludedCharacters().contains(character)) {
                continue;
            }

            if (character->isPlayer()) {
                character->send(message);
            } else {
                character->invokeTrigger("onsound", message);
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

            double multiplier;
            if (portal->isOpen()) {
                if (!portal->canHearThroughIfOpen()) {
                    continue;
                }
                multiplier = 1.0;
            } else {
                if (!portal->canHearThrough()) {
                    continue;
                }
                multiplier = portal->eventMultipliers()[GameEventType::SoundEvent];
            }

            Vector3D vector = room2->position() - room1->position();
            multiplier *= qMax(1.0 - 0.005 * vector.length(), 0.0);

            double propagatedStrength = strength * multiplier;
            if (propagatedStrength >= 0.1) {
                addVisit(oppositeRoom, propagatedStrength);
            }
        }
    }
}
