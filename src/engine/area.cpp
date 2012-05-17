#include "area.h"

#include "character.h"


Area::Area(uint id, Options options) :
    GameObject("area", id, options) {
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

    if (m_exits.removeAll(exit) > 0) {
        exit->setDeleted();

        setModified();
    }
}

void Area::setExits(const GameObjectPtrList &exits) {

    if (m_exits != exits) {
        m_exits = exits;

        setModified();
    }
}

void Area::addCharacter(const GameObjectPtr &character) {

    if (!m_characters.contains(character)) {
        m_characters << character;
    }
}

void Area::removeCharacter(const GameObjectPtr &character) {

    m_characters.removeAll(character);
}

void Area::setCharacters(const GameObjectPtrList &characters) {

    m_characters = characters;
}

void Area::addNPC(const GameObjectPtr &npc) {

    if (!m_npcs.contains(npc)) {
        m_npcs << npc;

        setModified();
    }
}

void Area::removeNPC(const GameObjectPtr &npc) {

    if (m_npcs.removeAll(npc) > 0) {
        setModified();
    }
}

void Area::setNPCs(const GameObjectPtrList &npcs) {

    if (m_npcs != npcs) {
        m_npcs = npcs;

        setModified();
    }
}

void Area::addItem(const GameObjectPtr &item) {

    if (!m_items.contains(item)) {
        m_items << item;

        setModified();
    }
}

void Area::removeItem(const GameObjectPtr &item) {

    if (m_items.removeAll(item) > 0) {
        setModified();
    }
}

void Area::setItems(const GameObjectPtrList &items) {

    if (m_items != items) {
        m_items = items;

        setModified();
    }
}
