#include "visualevent.h"


#define super GameEvent

VisualEvent::VisualEvent(Room *origin, double strength) :
    super(GameEventType::VisualEvent, origin, strength) {
}

VisualEvent::~VisualEvent() {
}

void VisualEvent::visitRoom(Room *room, double strength) {


}
