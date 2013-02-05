#include "soundevent.h"

#include "character.h"
#include "portal.h"
#include "room.h"


#define super GameEvent

SoundEvent::SoundEvent(Room *origin, double strength) :
    SoundEvent(GameEventType::Sound, origin, strength) {
}

SoundEvent::SoundEvent(GameEventType eventType, Room *origin, double strength) :
    super(eventType, origin, strength) {
}

SoundEvent::~SoundEvent() {
}

void SoundEvent::visitRoom(Room *room, double strength) {

    strength *= room->eventMultiplier(GameEventType::Sound);

    if (strength >= 0.1) {
        for (const GameObjectPtr &characterPtr : room->characters()) {
            if (excludedCharacters().contains(characterPtr)) {
                continue;
            }

            Character *character = characterPtr.cast<Character *>();
            QString message = descriptionForStrengthAndCharacterInRoom(strength, character, room);
            if (character->isPlayer()) {
                character->send(message);
            } else {
                character->invokeTrigger("onsound", message);
            }

            addAffectedCharacter(characterPtr);
        }

        for (const GameObjectPtr &portalPtr : room->portals()) {
            Portal *portal = portalPtr.cast<Portal *>();
            Room *room1 = portal->room().unsafeCast<Room *>();
            Room *room2 = portal->room2().unsafeCast<Room *>();
            Room *oppositeRoom = (room == room1 ? room2 : room1);
            if (hasBeenVisited(oppositeRoom) || !portal->canHearThrough()) {
                continue;
            }

            double propagatedStrength = strength * portal->eventMultiplier(GameEventType::Sound);
            if (propagatedStrength >= 0.1) {
                addVisit(oppositeRoom, propagatedStrength);
            }
        }
    }
}
