#include "area.h"


Area::Area(uint id, QObject *parent) :
    GameObject("area", id, parent) {
}

Area::~Area() {

    save();
}

void Area::setTitle(const QString &title) {

    m_title = title;

    setModified();
}

void Area::setDescription(const QString &description) {

    m_description = description;

    setModified();
}

void Area::addExit(const Exit &exit) {

    if (!m_exits.contains(exit)) {
        m_exits.append(exit);
    }

    setModified();
}

void Area::removeExit(const Exit &exit) {

    m_exits.removeAll(exit);

    setModified();
}

void Area::setExits(const ExitList &exits) {

    m_exits = exits;

    setModified();
}

void Area::addPresentCharacter(const GameObjectPtr &character) {

    if (!m_presentCharacters.contains(character)) {
        m_presentCharacters.append(character);
    }

    setModified();
}

void Area::removePresentCharacter(const GameObjectPtr &character) {

    m_presentCharacters.removeAll(character);

    setModified();
}

void Area::setPresentCharacters(const GameObjectPtrList &presentCharacters) {

    m_presentCharacters = presentCharacters;

    setModified();
}
