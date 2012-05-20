#include "character.h"

#include <QDebug>

#include "area.h"
#include "exit.h"
#include "player.h"
#include "util.h"


Character::Character(uint id, Options options) :
    Item("character", id, options),
    m_hp(1),
    m_gold(0) {
}

Character::Character(const char *objectType, uint id, Options options) :
    Item(objectType, id, options),
    m_hp(1),
    m_gold(0) {
}

Character::~Character() {
}

void Character::setName(const QString &newName) {

    Item::setName(newName);

    if (newName.toLower() != newName) {
        setIndefiniteArticle("");
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

void Character::adjustHp(int delta) {

    setHp(m_hp + delta);
}

void Character::setHp(int hp) {

    if (m_hp != hp) {
        m_hp = hp;
        if (m_hp < 0) {
            m_hp = 0;
        }

        setModified();
    }
}

void Character::adjustGold(int delta) {

    setGold(m_gold + delta);
}

void Character::setGold(int gold) {

    if (m_gold != gold) {
        m_gold = gold;
        if (m_gold < 0) {
            m_gold = 0;
        }

        setModified();
    }
}

void Character::open(const GameObjectPtr &exitPtr) {

    Exit *exit = exitPtr.cast<Exit *>();
    if (!exit) {
        qWarning() << "Character::open(): Invalid exit.";
        return;
    }

    if (!exit->isDoor()) {
        send("Exit cannot be opened.");
        return;
    }

    if (exit->isOpen()) {
        send(QString("The %1 is already open.").arg(exit->name()));
    } else {
        Area *area = currentArea().cast<Area *>();
        Q_ASSERT(area);

        exit->setOpen(true);
        send(QString("You open the %1.").arg(exit->name()));

        QString text = QString("%1 opens the %2.").arg(name(), exit->name());
        Util::sendOthers(area->players(), text, this);
    }
}

void Character::close(const GameObjectPtr &exitPtr) {

    Exit *exit = exitPtr.cast<Exit *>();
    if (!exit) {
        qWarning() << "Character::close(): Invalid exit.";
        return;
    }

    if (!exit->isDoor()) {
        send("Exit cannot be closed.");
        return;
    }

    if (exit->isOpen()) {
        Area *area = currentArea().cast<Area *>();
        Q_ASSERT(area);

        exit->setOpen(false);
        send(QString("You close the %1.").arg(exit->name()));

        QString text = QString("%1 closes the %2.").arg(name(), exit->name());
        Util::sendOthers(area->players(), text, this);
    } else {
        send(QString("The %1 is already closed.").arg(exit->name()));
    }
}

void Character::go(const GameObjectPtr &exitPtr) {

    Exit *exit = exitPtr.cast<Exit *>();
    if (!exit) {
        qWarning() << "Character::go(): Invalid exit.";
        return;
    }

    if (exit->isDoor() && !exit->isOpen()) {
        send(QString("The %1 is closed.").arg(exit->name()));
        return;
    }

    Area *area = currentArea().cast<Area *>();
    Q_ASSERT(area);
    foreach (const GameObjectPtr &character, area->npcs()) {
        if (character->invokeTrigger("onexit", QVariant::fromValue(GameObjectPtr(this)), exit->name())) {
            return;
        }
    }

    leave(currentArea(), exit->name());
    enter(exit->destinationArea());

    area = currentArea().cast<Area *>();
    Q_ASSERT(area);
    foreach (const GameObjectPtr &character, area->npcs()) {
        character->invokeTrigger("onenter", QVariant::fromValue(GameObjectPtr(this)));
    }
}

void Character::enter(const GameObjectPtr &areaPtr) {

    Area *area = areaPtr.cast<Area *>();
    Q_ASSERT(area);

    setCurrentArea(area);

    area->addNPC(this);

    QString article = indefiniteArticle();
    QString text = (article.isEmpty() ?
                    QString("%1 arrived.").arg(name()) :
                    QString("%1 %2 arrived.").arg(Util::capitalize(article), name()));
    Util::sendOthers(area->players(), text);
}

void Character::leave(const GameObjectPtr &areaPtr, const QString &exitName) {

    Area *area = areaPtr.cast<Area *>();
    Q_ASSERT(area);

    area->removeNPC(this);

    if (area->players().length() > 0) {
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
        Util::sendOthers(area->players(), text);
    }
}

void Character::say(const QString &message) {

    Area *area = currentArea().cast<Area *>();
    Q_ASSERT(area);

    QString text = QString("%1 says, \"%2\".").arg(name(), message);
    Util::sendOthers(area->players(), text);
}

void Character::shout(const QString &message) {

    Area *area = currentArea().cast<Area *>();
    Q_ASSERT(area);

    GameObjectPtrList players = area->players();
    foreach (const GameObjectPtr &exitPtr, area->exits()) {
        Exit *exit = exitPtr.cast<Exit *>();
        Q_ASSERT(exit);

        Area *adjacentArea = exit->destinationArea().cast<Area *>();
        Q_ASSERT(adjacentArea);

        players += adjacentArea->players();
    }

    QString text = QString("%1 shouts, \"%2\".").arg(name(), message);
    Util::sendOthers(players, text);
}

void Character::talk(const GameObjectPtr &characterPtr, const QString &message) {

    Character *character = characterPtr.cast<Character *>();
    if (!character) {
        qWarning() << "Character::talk(): Invalid character.";
        return;
    }

    Player *player = qobject_cast<Player *>(character);
    if (player) {
        tell(player, message);
    } else {
        character->invokeTrigger("ontalk", QVariant::fromValue(GameObjectPtr(this)), message);
    }
}

void Character::tell(const GameObjectPtr &playerPtr, const QString &message) {

    Player *player = playerPtr.cast<Player *>();
    if (!player) {
        qWarning() << "Character::tell(): Invalid player.";
        return;
    }

    if (message.isEmpty()) {
        send(QString("Tell %1 what?").arg(player->name()));
        return;
    }

    player->send(QString("%1 tells you, \"%2\".").arg(name(), message));
    send(QString("You tell %1, \"%2\".").arg(player->name(), message));
}
