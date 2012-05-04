#include "area.h"


Area::Area(uint id, QObject *parent) :
    GameObject("area", id, parent) {
}

Area::~Area() {

    save();
}

void Area::setTitle(const QString &title) {

    if (m_title != title) {
        m_title = title;

        setModified();
    }
}

void Area::setDescription(const QString &description) {

    if (m_description != description) {
        m_description = description;

        setModified();
    }
}

void Area::addExit(const Exit &exit) {

    if (!m_exits.contains(exit)) {
        m_exits << exit;

        setModified();
    }
}

void Area::removeExit(const Exit &exit) {

    if (m_exits.removeAll(exit) > 0) {
        setModified();
    }
}

void Area::setExits(const ExitList &exits) {

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
