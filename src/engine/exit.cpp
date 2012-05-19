#include "exit.h"


Exit::Exit(uint id, Options options) :
    GameObject("exit", id, options),
    m_size(1.5),
    m_door(false),
    m_open(false),
    m_hidden(false) {
}

Exit::~Exit() {
}

void Exit::setDestinationArea(const GameObjectPtr &destinationArea) {

    if (m_destinationArea != destinationArea) {
        m_destinationArea = destinationArea;

        setModified();
    }
}

void Exit::setSize(double size) {

    if (m_size != size) {
        m_size = size;

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

        if (open && isDoor()) {
            m_doorTimer.start(30 * 1000, this);
        }
    }
}

void Exit::setHidden(bool hidden) {

    if (m_hidden != hidden) {
        m_hidden = hidden;

        setModified();
    }
}

void Exit::timerEvent(QTimerEvent *event) {

    Q_UNUSED(event);

    m_open = false;
    m_doorTimer.stop();
}
