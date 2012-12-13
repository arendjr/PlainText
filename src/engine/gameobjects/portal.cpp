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

void Portal::setEventMultipliers(const GameEventMultiplierMap &multipliers) {

    if (m_eventMultipliers != multipliers) {
        m_eventMultipliers = multipliers;

        setModified();
    }
}

GameObjectPtr Portal::oppositeOf(const GameObjectPtr &room) const {

    if (room == m_room2) {
        return m_room;
    }
    return m_room2;
}

QString Portal::nameFromRoom(const GameObjectPtr &room) const {

    if (room == m_room2 && !m_name2.isEmpty()) {
        return m_name2;
    }
    return name();
}

QString Portal::descriptionFromRoom(const GameObjectPtr &room) const {

    if (room == m_room2 && (!m_name2.isEmpty() || !m_description2.isEmpty())) {
        return m_description2;
    }
    return description();
}

bool Portal::isHiddenFromRoom(const GameObjectPtr &room) const {

    if (room == m_room2) {
        return m_flags & PortalFlags::IsHiddenFromSide2;
    }
    return m_flags & PortalFlags::IsHiddenFromSide1;
}

bool Portal::canOpenFromRoom(const GameObjectPtr &room) const {

    if (room == m_room2) {
        return m_flags & PortalFlags::CanOpenFromSide2;
    }
    return m_flags & PortalFlags::CanOpenFromSide1;
}

bool Portal::canSeeThrough() const {

    return m_flags & PortalFlags::CanSeeThrough;
}

bool Portal::canHearThrough() const {

    return m_flags & PortalFlags::CanHearThrough;
}

bool Portal::canShootThrough() const {

    return m_flags & PortalFlags::CanShootThrough;
}

bool Portal::canPassThrough() const {

    return m_flags & PortalFlags::CanPassThrough;
}

bool Portal::canSeeThroughIfOpen() const {

    return m_flags & PortalFlags::CanSeeThroughIfOpen;
}

bool Portal::canHearThroughIfOpen() const {

    return m_flags & PortalFlags::CanHearThroughIfOpen;
}

bool Portal::canShootThroughIfOpen() const {

    return m_flags & PortalFlags::CanShootThroughIfOpen;
}

bool Portal::canPassThroughIfOpen() const {

    return m_flags & PortalFlags::CanPassThroughIfOpen;
}

void Portal::setOpen(bool open) {

    if (open) {
        setFlags(m_flags | PortalFlags::IsOpen);
    } else {
        setFlags(m_flags & ~PortalFlags::IsOpen);
    }
}
