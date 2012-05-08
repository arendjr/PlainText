#include "area.h"
#include "character.h"

#include "engine/util.h"


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

void Area::enter(Character *character) {

    character->setCurrentArea(this);

    addCharacter(character);

    if (!name().isEmpty()) {
        character->send(Util::colorize("\n" + name() + "\n\n", Teal));
    }

    character->send(description());

    if (exits().length() > 0) {
        QStringList exitNames;
        foreach (const GameObjectPtr &exit, exits()) {
            exitNames << exit->name();
        }
        character->send(Util::colorize("Obvious exits: " + exitNames.join(", ") + ".", Green));
    }

    GameObjectPtrList others = characters();
    others.removeOne(character);
    if (others.length() > 0) {
        QStringList characterNames;
        foreach (const GameObjectPtr &otherPtr, others) {
            Character *other = otherPtr.cast<Character *>();
            Q_ASSERT(other);

            characterNames << other->name();

            other->send(QString("%1 arrived.").arg(character->name()));
        }
        character->send("You see " + Util::joinFancy(characterNames) + ".");
    }
}

void Area::leave(Character *character, const QString &exitName) {

    removeCharacter(character);

    foreach (const GameObjectPtr &otherPtr, characters()) {
        Character *other = otherPtr.cast<Character *>();
        Q_ASSERT(other);

        if (exitName.isEmpty()) {
            other->send(QString("%1 left.").arg(character->name()));
        } else {
            other->send(QString("%1 left to the %2.").arg(character->name(), exitName));
        }
    }
}

