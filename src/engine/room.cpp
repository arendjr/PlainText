#include "room.h"

#include "exit.h"
#include "item.h"
#include "portal.h"


#define super GameObject

Room::Room(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Room, id, options),
    m_position(0, 0, 0) {

    setAutoDelete(false);
}

Room::~Room() {
}

void Room::setPosition(const Point3D &position) {

    if (m_position != position) {
        m_position = position;

        setModified();
    }
}

void Room::addPortal(const GameObjectPtr &portal) {

    if (!m_portals.contains(portal)) {
        m_portals << portal;

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

GameObjectPtrList Room::exits() {

    GameObjectPtrList exits;
    for (const GameObjectPtr &portalPtr : m_portals) {
        Portal *portal = portalPtr.cast<Portal *>();
        if (portal->flags() & PortalFlags::CanPassThrough ||
            (portal->flags() & PortalFlags::CanPassThroughIfOpen &&
             portal->canOpenFromRoom(this))) {
            Exit *exit = new Exit(realm());
            exit->setName(portal->nameFromRoom(this));
            exit->setDescription(portal->descriptionFromRoom(this));
            exit->setDestination(portal->oppositeOf(this));
            exit->setDoor(portal->canOpenFromRoom(this));
            exit->setOpen(portal->isOpen());
            exit->setHidden(portal->isHiddenFromRoom(this));
            exits.append(exit);
        }
    }
    return exits;
}

void Room::addPlayer(const GameObjectPtr &player) {

    if (!m_players.contains(player)) {
        m_players << player;
    }
}

void Room::removePlayer(const GameObjectPtr &player) {

    m_players.removeOne(player);
}

void Room::setPlayers(const GameObjectPtrList &players) {

    m_players = players;
}

void Room::addNPC(const GameObjectPtr &npc) {

    if (!m_npcs.contains(npc)) {
        m_npcs << npc;
    }
}

void Room::removeNPC(const GameObjectPtr &npc) {

    m_npcs.removeOne(npc);
}

void Room::setNPCs(const GameObjectPtrList &npcs) {

    if (m_npcs != npcs) {
        m_npcs = npcs;
    }
}

void Room::addItem(const GameObjectPtr &item) {

    if (!m_items.contains(item)) {
        m_items << item;

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

void Room::addGold(double amount) {

    GameObjectPtr goldPtr;
    for (const GameObjectPtr &itemPtr : m_items) {
        if (itemPtr->name().endsWith("worth of gold")) {
            goldPtr = itemPtr;
            break;
        }
    }

    if (goldPtr.isNull()) {
        Item *gold = Item::createGold(realm(), amount);
        addItem(gold);
    } else {
        Item *gold = goldPtr.cast<Item *>();
        gold->setCost(gold->cost() + amount);
        gold->setName(QString("$%1 worth of gold").arg(gold->cost()));
    }
}
