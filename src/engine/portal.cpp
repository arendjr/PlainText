#include "portal.h"


Portal::Portal(Realm *realm, uint id, Options options) :
    GameObject(realm, GameObjectType::Portal, id, options),
    m_flags(PortalFlags::NoFlags) {
}

Portal::~Portal() {
}

void Portal::setName2(const QString &name2) {

    if (m_name2 != name2) {
        m_name2 = name2;

        setModified();
    }
}

void Portal::setDescription2(const QString &description2) {

    if (m_description2 != description2) {
        m_description2 = description2;

        setModified();
    }
}

void Portal::setRoom(const GameObjectPtr &room) {

    if (m_room != room) {
        m_room = room;

        setModified();
    }
}

void Portal::setRoom2(const GameObjectPtr &room2) {

    if (m_room2 != room2) {
        m_room2 = room2;

        setModified();
    }
}

void Portal::setFlags(PortalFlags flags) {

    if (m_flags != flags) {
        m_flags = flags;

        setModified();
    }
}

QString Portal::nameFromRoom(const GameObjectPtr &room) {

    if (room == m_room2 && !m_name2.isEmpty()) {
        return m_name2;
    }
    return name();
}

QString Portal::descriptionFromRoom(const GameObjectPtr &room) {

    if (room == m_room2 && (!m_name2.isEmpty() || !m_description2.isEmpty())) {
        return m_description2;
    }
    return description();
}

bool Portal::isHiddenFromRoom(const GameObjectPtr &room) {

    if (room == m_room2) {
        return m_flags & PortalFlags::IsHiddenFromSide2;
    }
    return m_flags & PortalFlags::IsHiddenFromSide1;
}

bool Portal::canOpenFromRoom(const GameObjectPtr &room) {

    if (room == m_room2) {
        return m_flags & PortalFlags::CanOpenFromSide2;
    }
    return m_flags & PortalFlags::CanOpenFromSide1;
}
