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
        QString message;

        if (strength >= 0.7 || distantDescription().isEmpty()) {
            message = description();
        } else if (strength >= 0.4 || veryDistantDescription().isEmpty()) {
            message = distantDescription();
        } else {
            message = veryDistantDescription();
        }

        for (const GameObjectPtr &character : room->characters()) {
            if (excludedCharacters().contains(character)) {
                continue;
            }

            if (character->isPlayer()) {
                character->send(message);
            } else {
                character->invokeTrigger("onsound", message);
            }
        }

        for (const GameObjectPtr &portalPtr : room->portals()) {
            Portal *portal = portalPtr.cast<Portal *>();

            double multiplier;
            if (portal->isOpen()) {
                if (!portal->canHearThrough()) {
                    continue;
                }
                multiplier = 1.0;
            } else {
                if (!portal->canHearThroughIfOpen()) {
                    continue;
                }
                multiplier = portal->eventMultipliers()[GameEventType::SoundEvent];
            }

            Room *room = portal->room().cast<Room *>();
            Room *room2 = portal->room2().cast<Room *>();
            Vector3D vector = room2->position() - room->position();
            multiplier *= 1.0 - vector.length() / 50.0;
        }
    }
}
