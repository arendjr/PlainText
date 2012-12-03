#include "movementevent.h"


#define super VisualEvent

MovementEvent::MovementEvent(Room *origin, double strength) :
    super(origin, strength) {
}

MovementEvent::~MovementEvent() {
}
