#include "room.h"

#include "character.h"


#define super GameObject

Room::Room(Realm *realm, uint id, Options options) :
    super(realm, "room", id, options),
    m_position(0, 0, 0) {
}

Room::~Room() {
}

void Room::setPosition(const Point3D &position) {

    if (m_position != position) {
        m_position = position;

        setModified();
    }
}

void Room::addExit(const GameObjectPtr &exit) {

    if (!m_exits.contains(exit)) {
        m_exits << exit;

        setModified();
    }
}

void Room::removeExit(const GameObjectPtr &exit) {

    if (m_exits.removeOne(exit)) {
        setModified();
    }
}

void Room::setExits(const GameObjectPtrList &exits) {

    if (m_exits != exits) {
        m_exits = exits;

        setModified();
    }
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

void Room::setVisibleRooms(const GameObjectPtrList &visibleRooms) {

    if (m_visibleRooms != visibleRooms) {
        m_visibleRooms = visibleRooms;

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
