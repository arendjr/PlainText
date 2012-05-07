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

void Area::addPresentCharacter(const GameObjectPtr &character) {

    if (!m_presentCharacters.contains(character)) {
        m_presentCharacters << character;
    }
}

void Area::removePresentCharacter(const GameObjectPtr &character) {

    m_presentCharacters.removeAll(character);
}

void Area::setPresentCharacters(const GameObjectPtrList &presentCharacters) {

    m_presentCharacters = presentCharacters;
}
