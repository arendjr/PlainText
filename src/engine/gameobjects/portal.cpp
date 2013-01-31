#include "portal.h"

#include "room.h"


#define super GameObject

Portal::Portal(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Portal, id, options),
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

void Portal::setDestination(const QString &destination) {

    if (m_destination != destination) {
        m_destination = destination;

        setModified();
    }
}

void Portal::setDestination2(const QString &destination2) {

    if (m_destination2 != destination2) {
        m_destination2 = destination2;

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

Point3D Portal::position() const {

    Point3D position1 = m_room.cast<Room *>()->position();
    Point3D position2 = m_room2.cast<Room *>()->position();
    return Point3D((position1.x + position2.x) / 2,
                   (position1.y + position2.y) / 2,
                   (position1.z + position2.z) / 2);
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

double Portal::eventMultiplier(GameEventType eventType) const {

    double multiplier = (m_flags & PortalFlags::IsOpen ? 1.0 : m_eventMultipliers[eventType]);

    Vector3D vector = m_room.unsafeCast<Room *>()->position() -
                      m_room2.unsafeCast<Room *>()->position();
    if (eventType == GameEventType::Sound) {
        multiplier *= qMax(1.0 - 0.05 * vector.length(), 0.0);
    } else if (eventType == GameEventType::Visual) {
        multiplier *= qMax(1.0 - 0.0005 * vector.length(), 0.0);
    }

    return multiplier;
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

QString Portal::destinationFromRoom(const GameObjectPtr &room) const {

    if (room == m_room2) {
        return m_destination2;
    }
    return m_destination;
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

bool Portal::canOpen() const {

    return m_flags & (PortalFlags::CanOpenFromSide1 | PortalFlags::CanOpenFromSide2);
}

bool Portal::canSeeThrough() const {

    return m_flags & PortalFlags::IsOpen ? m_flags & PortalFlags::CanSeeThroughIfOpen :
                                           m_flags & PortalFlags::CanSeeThrough;
}

bool Portal::canHearThrough() const {

    return m_flags & PortalFlags::IsOpen ? m_flags & PortalFlags::CanHearThroughIfOpen :
                                           m_flags & PortalFlags::CanHearThrough;
}

bool Portal::canShootThrough() const {

    return m_flags & PortalFlags::IsOpen ? m_flags & PortalFlags::CanShootThroughIfOpen :
                                           m_flags & PortalFlags::CanShootThrough;
}

bool Portal::canPassThrough() const {

    return m_flags & PortalFlags::IsOpen ? m_flags & PortalFlags::CanPassThroughIfOpen :
                                           m_flags & PortalFlags::CanPassThrough;
}

void Portal::setOpen(bool open) {

    if (open) {
        setFlags(m_flags | PortalFlags::IsOpen);
    } else {
        setFlags(m_flags & ~PortalFlags::IsOpen);
    }
}
