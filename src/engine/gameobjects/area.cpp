#include "area.h"


#define super GameObject

Area::Area(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Area, id, options) {
}

Area::~Area() {
}

void Area::addRoom(const GameObjectPtr &room) {

    if (!m_rooms.contains(room)) {
        m_rooms.append(room);

        setModified();
    }
}

void Area::removeRoom(const GameObjectPtr &room) {

    if (m_rooms.removeOne(room)) {
        setModified();
    }
}

void Area::setRooms(const GameObjectPtrList &rooms) {

    if (m_rooms != rooms) {
        m_rooms = rooms;

        setModified();
    }
}
