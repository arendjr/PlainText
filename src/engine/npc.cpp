#include "npc.h"

#include "area.h"
#include "character.h"
#include "util.h"


NPC::NPC(uint id, Options options) :
    Item("npc", id, options),
    m_hp(1) {
}

NPC::~NPC() {
}

void NPC::setName(const QString &newName) {

    Item::setName(newName);

    if (newName.toLower() != newName) {
        setIndefiniteArticle("");
    }
}

void NPC::setCurrentArea(const GameObjectPtr &currentArea) {

    if (m_currentArea != currentArea) {
        m_currentArea = currentArea;

        setModified();
    }
}

void NPC::addInventoryItem(const GameObjectPtr &item) {

    if (!m_inventory.contains(item)) {
        m_inventory << item;

        setModified();
    }
}

void NPC::removeInventoryItem(const GameObjectPtr &item) {

    if (m_inventory.removeAll(item) > 0) {
        setModified();
    }
}

void NPC::setInventory(const GameObjectPtrList &inventory) {

    if (m_inventory != inventory) {
        m_inventory = inventory;

        setModified();
    }
}

void NPC::setHp(int hp) {

    if (m_hp != hp) {
        m_hp = hp;

        setModified();
    }
}

void NPC::enter(const GameObjectPtr &areaPtr) {

    Area *area = areaPtr.cast<Area *>();
    Q_ASSERT(area);

    setCurrentArea(area);

    area->addNPC(this);

    QString article = indefiniteArticle();
    QString text = (article.isEmpty() ?
                    QString("%1 arrived.").arg(name()) :
                    QString("%1 %2 arrived.").arg(Util::capitalize(article), name()));

    foreach (const GameObjectPtr &characterPtr, area->characters()) {
        Character *character = characterPtr.cast<Character *>();
        Q_ASSERT(character);

        character->send(text);
    }
}

void NPC::leave(const GameObjectPtr &areaPtr, const QString &exitName) {

    Area *area = areaPtr.cast<Area *>();
    Q_ASSERT(area);

    area->removeNPC(this);

    if (area->characters().length() > 0) {
        QString text;
        QString article = indefiniteArticle();
        if (article.isEmpty()) {
            text = (exitName.isEmpty() ?
                    QString("%1 left.").arg(name()) :
                    QString("%1 left to the %2.").arg(name(), exitName));
        } else {
            bool unique = true;
            foreach (const GameObjectPtr &other, area->npcs()) {
                if (other->name() == name()) {
                    unique = false;
                    break;
                }
            }
            if (unique) {
                text = (exitName.isEmpty() ?
                        QString("The %1 left.").arg(name()) :
                        QString("The %1 left to the %2.").arg(name(), exitName));
            } else {
                text = (exitName.isEmpty() ?
                        QString("%1 %2 left.").arg(Util::capitalize(article), name()) :
                        QString("%1 %2 left to the %3.").arg(Util::capitalize(article), name(), exitName));
            }
        }

        foreach (const GameObjectPtr &characterPtr, area->characters()) {
            Character *character = characterPtr.cast<Character *>();
            Q_ASSERT(character);

            character->send(text);
        }
    }
}

