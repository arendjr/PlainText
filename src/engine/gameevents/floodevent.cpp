#include "floodevent.h"

#include <QDebug>

#include "character.h"
#include "portal.h"
#include "room.h"
#include "util.h"
#include "vector3d.h"


#define super GameEvent

FloodEvent::FloodEvent(Room *origin, double strength) :
    super(GameEventType::Flood, origin, strength) {
}

FloodEvent::~FloodEvent() {
}

QString FloodEvent::descriptionForStrengthAndCharacterInRoom(double strength, Room *room) const {

    Q_UNUSED(room)

    if (strength >= 1.0 || distantDescription().isEmpty()) {
        return description();
    } else if (strength >= 0.2 || veryDistantDescription().isEmpty()) {
        return distantDescription();
    } else {
        return veryDistantDescription();
    }
}

void FloodEvent::visitRoom(Room *room, double strength) {

    QString message = descriptionForStrengthAndCharacterInRoom(strength - room->position().z, room);

    room->setDescription(room->description() + "\n" + message);

    for (const GameObjectPtr &characterPtr : room->characters()) {
        if (excludedCharacters().contains(characterPtr)) {
            continue;
        }

        if (characterPtr->isPlayer()) {
            characterPtr->send(message);
        } else {
            characterPtr->invokeTrigger("onflood", message);
        }
        addAffectedCharacter(characterPtr);
    }

    for (const GameObjectPtr &portalPtr : room->portals()) {
        Portal *portal = portalPtr.unsafeCast<Portal *>();
        Room *room1 = portal->room().unsafeCast<Room *>();
        Room *room2 = portal->room2().unsafeCast<Room *>();
        Room *oppositeRoom = (room == room1 ? room2 : room1);
        if (hasBeenVisited(oppositeRoom) || !portal->canPassThrough()) {
            continue;
        }

        if (oppositeRoom->position().z > strength) {
            continue;
        }

        addVisit(oppositeRoom, strength);
    }
}
