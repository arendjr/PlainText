#include "areaevent.h"

#include "area.h"
#include "room.h"


#define super GameEvent

AreaEvent::AreaEvent(Room *origin, double strength) :
    super(GameEventType::AreaEvent, origin, strength) {

    if (!origin->area().isNull()) {
        Area *area = origin->area().cast<Area *>();
        for (const GameObjectPtr &room : area->rooms()) {
            if (room != origin) {
                addVisit(room.cast<Room *>(), strength);
            }
        }
    }
}

AreaEvent::~AreaEvent() {
}

void AreaEvent::visitRoom(Room *room, double strength) {

    QString message = descriptionForStrengthInRoom(strength, room);

    for (const GameObjectPtr &character : room->characters()) {
        if (excludedCharacters().contains(character)) {
            continue;
        }

        character->send(message);

        addAffectedCharacter(character);
    }
}
