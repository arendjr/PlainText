#include "area.h"

#include "character.h"


Area::Area(Realm *realm, uint id, Options options) :
    GameObject(realm, "area", id, options) {
}

Area::~Area() {
}

void Area::addExit(const GameObjectPtr &exit) {

    if (!m_exits.contains(exit)) {
        m_exits << exit;

        setModified();
    }
}

void Area::removeExit(const GameObjectPtr &exit) {

    if (m_exits.removeOne(exit)) {
        setModified();
    }
}

void Area::setExits(const GameObjectPtrList &exits) {

    if (m_exits != exits) {
        m_exits = exits;

        setModified();
    }
}

void Area::addPlayer(const GameObjectPtr &player) {

    if (!m_players.contains(player)) {
        m_players << player;
    }
}

void Area::removePlayer(const GameObjectPtr &player) {

    m_players.removeOne(player);
}

void Area::setPlayers(const GameObjectPtrList &players) {

    m_players = players;
}

void Area::addNPC(const GameObjectPtr &npc) {

    if (!m_npcs.contains(npc)) {
        m_npcs << npc;
    }
}

void Area::removeNPC(const GameObjectPtr &npc) {

    m_npcs.removeOne(npc);
}

void Area::setNPCs(const GameObjectPtrList &npcs) {

    if (m_npcs != npcs) {
        m_npcs = npcs;
    }
}

void Area::addItem(const GameObjectPtr &item) {

    if (!m_items.contains(item)) {
        m_items << item;

        setModified();
    }
}

void Area::removeItem(const GameObjectPtr &item) {

    if (m_items.removeOne(item)) {
        setModified();
    }
}

void Area::setItems(const GameObjectPtrList &items) {

    if (m_items != items) {
        m_items = items;

        setModified();
    }
}
