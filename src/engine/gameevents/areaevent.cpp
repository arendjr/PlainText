#include "areaevent.h"

#include "area.h"
#include "character.h"
#include "room.h"


#define super GameEvent

AreaEvent::AreaEvent(Room *origin, double strength) :
    super(GameEventType::Area, origin, strength) {

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

    for (const GameObjectPtr &characterPtr : room->characters()) {
        if (excludedCharacters().contains(characterPtr)) {
            continue;
        }

        Character *character = characterPtr.cast<Character *>();
        QString message = descriptionForStrengthAndCharacterInRoom(strength, character, room);
        character->send(message);

        addAffectedCharacter(characterPtr);
    }
}
