#include "character.h"

#include "area.h"
#include "realm.h"
#include "util.h"


Character::Character(uint id, Options options) :
    GameObject("character", id, options),
    m_hp(1),
    m_admin(false),
    m_session(0) {
}

Character::~Character() {

    if (~options() & Copy) {
        Realm::instance()->unregisterCharacter(this);
    }
}

void Character::setName(const QString &newName) {

    Q_ASSERT(name().isEmpty());

    GameObject::setName(newName);

    if (~options() & Copy) {
        Realm::instance()->registerCharacter(this);
    }
}

void Character::setPasswordHash(const QString &passwordHash) {

    if (m_passwordHash != passwordHash) {
        m_passwordHash = passwordHash;

        setModified();
    }
}

void Character::setCurrentArea(const GameObjectPtr &currentArea) {

    if (m_currentArea != currentArea) {
        m_currentArea = currentArea;

        setModified();
    }
}

void Character::addInventoryItem(const GameObjectPtr &item) {

    if (!m_inventory.contains(item)) {
        m_inventory << item;

        setModified();
    }
}

void Character::removeInventoryItem(const GameObjectPtr &item) {

    if (m_inventory.removeAll(item) > 0) {
        setModified();
    }
}

void Character::setInventory(const GameObjectPtrList &inventory) {

    if (m_inventory != inventory) {
        m_inventory = inventory;

        setModified();
    }
}

void Character::setHp(int hp) {

    if (m_hp != hp) {
        m_hp = hp;

        setModified();
    }
}

void Character::setAdmin(bool admin) {

    if (m_admin != admin) {
        m_admin = admin;

        setModified();
    }
}

void Character::setSession(Session *session) {

    m_session = session;
}

void Character::send(QString data) {

    if (!data.endsWith("\n")) {
        data += "\n";
    }
    write(data);
}

void Character::enter(const GameObjectPtr &areaPtr) {

    Area *area = areaPtr.cast<Area *>();
    Q_ASSERT(area);

    setCurrentArea(area);

    foreach (const GameObjectPtr &otherPtr, area->characters()) {
        Character *other = otherPtr.cast<Character *>();
        Q_ASSERT(other);

        other->send(QString("%1 arrived.").arg(name()));
    }

    area->addCharacter(this);

    look();
}

void Character::look() {

    Area *area = currentArea().cast<Area *>();
    QString text;

    if (!area->name().isEmpty()) {
        text += "\n" + Util::colorize(area->name(), Teal) + "\n\n";
    }

    text += area->description() + "\n";

    if (area->exits().length() > 0) {
        QStringList exitNames;
        foreach (const GameObjectPtr &exit, area->exits()) {
            exitNames << exit->name();
        }
        text += Util::colorize("Obvious exits: " + exitNames.join(", ") + ".", Green) + "\n";
    }

    GameObjectPtrList others = area->characters();
    others.removeOne(this);
    if (others.length() > 0) {
        QStringList characterNames;
        foreach (const GameObjectPtr &other, others) {
            characterNames << other->name();
        }
        text += "You see " + Util::joinFancy(characterNames) + ".\n";
    }

    if (area->npcs().length() > 0) {
        text += "You see " + Util::joinItems(area->npcs()) + ".\n";
    }

    if (area->items().length() > 0) {
        text += "You see " + Util::joinItems(area->items()) + ".\n";
    }

    send(text);
}

void Character::leave(const GameObjectPtr &areaPtr, const QString &exitName) {

    Area *area = areaPtr.cast<Area *>();
    Q_ASSERT(area);

    area->removeCharacter(this);

    foreach (const GameObjectPtr &otherPtr, area->characters()) {
        Character *other = otherPtr.cast<Character *>();
        Q_ASSERT(other);

        if (exitName.isEmpty()) {
            other->send(QString("%1 left.").arg(name()));
        } else {
            other->send(QString("%1 left to the %2.").arg(name(), exitName));
        }
    }
}

