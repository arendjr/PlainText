#include "exit.h"


Exit::Exit(Realm *realm, uint id, Options options) :
    GameObject(realm, GameObjectType::Exit, id, options),
    m_door(false),
    m_open(false),
    m_hidden(false) {
}

Exit::~Exit() {
}

void Exit::setDestination(const GameObjectPtr &destinationArea) {

    if (m_destination != destinationArea) {
        m_destination = destinationArea;

        setModified();
    }
}

void Exit::setOppositeExit(const GameObjectPtr &oppositeExit) {

    if (m_oppositeExit != oppositeExit) {
        m_oppositeExit = oppositeExit;

        setModified();
    }
}

void Exit::setDoor(bool door) {

    if (m_door != door) {
        m_door = door;

        setModified();
    }
}

void Exit::setOpen(bool open) {

    if (m_open != open) {
        m_open = open;
    }
}

void Exit::setHidden(bool hidden) {

    if (m_hidden != hidden) {
        m_hidden = hidden;

        setModified();
    }
}
