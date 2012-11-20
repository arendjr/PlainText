#include "exit.h"


#define super GameObject

Exit::Exit(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Exit, id, options | DontSave),
    m_door(false),
    m_open(false),
    m_hidden(false) {
}

Exit::~Exit() {
}

void Exit::setDestination(const GameObjectPtr &destination) {

    m_destination = destination;
}

void Exit::setDoor(bool door) {

    m_door = door;
}

void Exit::setOpen(bool open) {

    m_open = open;
}

void Exit::setHidden(bool hidden) {

    m_hidden = hidden;
}
