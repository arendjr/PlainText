#include "soundevent.h"


#define super GameEvent

SoundEvent::SoundEvent(Room *origin, double strength) :
    super(GameEventType::SoundEvent, origin, strength) {
}

SoundEvent::~SoundEvent() {
}

void SoundEvent::visitRoom(Room *room, double strength) {


}
