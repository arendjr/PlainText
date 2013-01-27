#include "room.h"

#include "item.h"
#include "portal.h"
#include "util.h"


#define super GameObject

Room::Room(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Room, id, (Options) (options | NeverDelete)),
    m_type(RoomType::Room),
    m_position(0, 0, 0),
    m_flags(RoomFlags::NoFlags),
    m_portals(8) {
}

Room::~Room() {
}

void Room::setArea(const GameObjectPtr &area) {

    if (m_area != area) {
        m_area = area;

        setModified();
    }
}

void Room::setType(RoomType type) {

    if (m_type != type) {
        m_type = type;

        setModified();
    }
}

void Room::setPosition(const Point3D &position) {

    if (m_position != position) {
        m_position = position;

        setModified();
    }
}

void Room::setFlags(RoomFlags flags) {

    if (m_flags != flags) {
        m_flags = flags;

        setModified();
    }
}

void Room::addPortal(const GameObjectPtr &portal) {

    if (!m_portals.contains(portal)) {
        m_portals.append(portal);

        setModified();
    }
}

void Room::removePortal(const GameObjectPtr &portal) {

    if (m_portals.removeOne(portal)) {
        setModified();
    }
}

void Room::setPortals(const GameObjectPtrList &portals) {

    if (m_portals != portals) {
        m_portals = portals;

        setModified();
    }
}

void Room::addCharacter(const GameObjectPtr &character) {

    if (!m_characters.contains(character)) {
        m_characters.append(character);
    }
}

void Room::removeCharacter(const GameObjectPtr &character) {

    m_characters.removeOne(character);
}

void Room::setCharacters(const GameObjectPtrList &characters) {

    if (m_characters != characters) {
        m_characters = characters;
    }
}

void Room::addItem(const GameObjectPtr &item) {

    if (!m_items.contains(item)) {
        m_items.append(item);

        setModified();
    }
}

void Room::removeItem(const GameObjectPtr &item) {

    if (m_items.removeOne(item)) {
        setModified();
    }
}

void Room::setItems(const GameObjectPtrList &items) {

    if (m_items != items) {
        m_items = items;

        setModified();
    }
}

void Room::setEventMultipliers(const GameEventMultiplierMap &multipliers) {

    if (m_eventMultipliers != multipliers) {
        m_eventMultipliers = multipliers;

        setModified();
    }
}
