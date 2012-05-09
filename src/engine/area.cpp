#include "area.h"


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
