#include "character.h"

#include <cstdlib>

#include <QDebug>
#include <QTimerEvent>

#include "area.h"
#include "exit.h"
#include "player.h"
#include "util.h"


#define NO_STUN \
    if (m_secondsStunned > 0) { \
        send(Util::colorize(QString("Please wait %1 seconds.").arg(m_secondsStunned), Olive)); \
        return; \
    }


Character::Character(uint id, Options options) :
    Item("character", id, options),
    m_gender("male"),
    m_subjectPronoun("he"),
    m_objectPronoun("him"),
    m_hp(1),
    m_maxHp(1),
    m_mp(0),
    m_maxMp(0),
    m_gold(0),
    m_secondsStunned(0),
    m_stunTimeout(0),
    m_oddStunTimer(false),
    m_leaveOnActive(false) {

    setAutoDelete(false);
}

Character::Character(const char *objectType, uint id, Options options) :
    Item(objectType, id, options),
    m_gender("male"),
    m_subjectPronoun("he"),
    m_objectPronoun("him"),
    m_hp(1),
    m_maxHp(1),
    m_mp(0),
    m_maxMp(0),
    m_gold(0),
    m_secondsStunned(0),
    m_stunTimeout(0),
    m_oddStunTimer(false),
    m_leaveOnActive(false) {

    setAutoDelete(false);
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

void Character::setRace(const GameObjectPtr &race) {

    if (m_race != race) {
        m_race = race;

        setModified();
    }
}

void Character::setClass(const GameObjectPtr &characterClass) {

    if (m_class != characterClass) {
        m_class = characterClass;

        setModified();
    }
}

void Character::setGender(const QString &gender) {

    if (m_gender != gender) {
        m_gender = gender;
        m_subjectPronoun = (m_gender == "male" ? "he" : "she");
        m_objectPronoun = (m_gender == "male" ? "him" : "her");

        setModified();
    }
}

void Character::setStats(const CharacterStats &stats) {

    if (m_stats != stats) {
        m_stats = stats;

        startBulkModification();

        setMaxHp(2 * m_stats.vitality);
        setMaxMp(m_stats.intelligence);

        commitBulkModification();
    }
}

void Character::adjustHp(int delta) {

    setHp(m_hp + delta);
}

void Character::setHp(int hp) {

    if (m_hp != hp) {
        m_hp = qBound(0, hp, maxHp());

        setModified();
    }
}

void Character::setMaxHp(int maxHp) {

    if (m_maxHp != maxHp) {
        m_maxHp = qMax(maxHp, 0);

        setModified();
    }
}

void Character::adjustMp(int delta) {

    setMp(m_mp + delta);
}

void Character::setMp(int mp) {

    if (m_mp != mp) {
        m_mp = qBound(0, mp, maxMp());

        setModified();
    }
}

void Character::setMaxMp(int maxMp) {

    if (m_maxMp != maxMp) {
        m_maxMp = qMax(maxMp, 0);

        setModified();
    }
}

void Character::adjustGold(int delta) {

    setGold(m_gold + delta);
}

void Character::setGold(int gold) {

    if (m_gold != gold) {
        m_gold = qMax(gold, 0);

        setModified();
    }
}

void Character::open(const GameObjectPtr &exitPtr) {

    NO_STUN

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
        if (!exit->invokeTrigger("onopen", this)) {
            return;
        }

        exit->setOpen(true);
        send(QString("You open the %1.").arg(exit->name()));

        QString text = QString("%1 opens the %2.").arg(name(), exit->name());
        Util::sendOthers(currentArea().cast<Area *>()->players(), text, this);
    }
}

void Character::close(const GameObjectPtr &exitPtr) {

    NO_STUN

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
        if (!exit->invokeTrigger("onclose", this)) {
            return;
        }

        exit->setOpen(false);
        send(QString("You close the %1.").arg(exit->name()));

        QString text = QString("%1 closes the %2.").arg(name(), exit->name());
        Util::sendOthers(currentArea().cast<Area *>()->players(), text, this);
    } else {
        send(QString("The %1 is already closed.").arg(exit->name()));
    }
}

void Character::go(const GameObjectPtr &exitPtr) {

    NO_STUN

    Exit *exit = exitPtr.cast<Exit *>();

    if (exit->isDoor() && !exit->isOpen()) {
        send(QString("The %1 is closed.").arg(exit->name()));
        return;
    }

    Area *area = currentArea().cast<Area *>();
    foreach (const GameObjectPtr &character, area->npcs()) {
        if (!character->invokeTrigger("onexit", this, exit->name())) {
            return;
        }
    }

    if (!exit->invokeTrigger("onenter", this)) {
        return;
    }

    leave(currentArea(), exit->name());
    enter(exit->destinationArea());

    area = currentArea().cast<Area *>();
    foreach (const GameObjectPtr &character, area->npcs()) {
        character->invokeTrigger("onenter", this);
    }
}

void Character::enter(const GameObjectPtr &areaPtr) {

    Area *area = areaPtr.cast<Area *>();

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

    QString text = QString("%1 says, \"%2\".").arg(name(), message);
    Util::sendOthers(currentArea().cast<Area *>()->players(), text);
}

void Character::shout(const QString &message) {

    Area *area = currentArea().cast<Area *>();

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
    Player *player = qobject_cast<Player *>(character);
    if (player) {
        tell(player, message);
    } else {
        character->invokeTrigger("ontalk", this, message);
    }
}

void Character::tell(const GameObjectPtr &playerPtr, const QString &message) {

    Player *player = playerPtr.cast<Player *>();

    if (message.isEmpty()) {
        send(QString("Tell %1 what?").arg(player->name()));
        return;
    }

    player->send(QString("%1 tells you, \"%2\".").arg(name(), message));
    send(QString("You tell %1, \"%2\".").arg(player->name(), message));
}

void Character::kill(const GameObjectPtr &characterPtr) {

    NO_STUN

    Character *character = characterPtr.cast<Character *>();
    GameObjectPtrList others = currentArea().cast<Area *>()->players();

    QString myName = indefiniteArticle().isEmpty() ? name() : "The " + name();
    QString enemyName = (character->indefiniteArticle().isEmpty() ? "" : "the ") +
                        character->name();

    qreal hitChance = 100 * ((80 + stats().dexterity) / 160.0) *
                            ((100 - character->stats().dexterity) / 100.0);
    if (qrand() % 100 < hitChance) {
        int damage = qrand() % (int) (20.0 * (stats().strength / 40.0) *
                                             ((80 - character->stats().endurance) / 80.0)) + 1;

        character->adjustHp(-damage);

        switch (qrand() % 3) {
            case 0:
                character->send(Util::colorize(QString("%1 deals you a sweeping punch, causing %2 damage.").arg(myName).arg(damage), Maroon));
                send(Util::colorize(QString("You deal a sweeping punch to %1, causing %2 damage.").arg(enemyName).arg(damage), Teal));
                Util::sendOthers(others, Util::colorize(QString("%1 deals a sweeping punch to %2.").arg(myName, enemyName), Teal), this, character);
                break;
            case 1:
                character->send(Util::colorize(QString("%1 hits you on the jaw for %2 damage.").arg(myName).arg(damage), Maroon));
                send(Util::colorize(QString("You hit %1 on the jaw for %2 damage.").arg(enemyName).arg(damage), Teal));
                Util::sendOthers(others, Util::colorize(QString("%1 hits %2 on the jaw.").arg(myName, enemyName), Teal), this, character);
                break;
            case 2:
                character->send(Util::colorize(QString("%1 kicks you for %2 damage.").arg(myName).arg(damage), Maroon));
                send(Util::colorize(QString("You kick %1 for %2 damage.").arg(enemyName).arg(damage), Teal));
                Util::sendOthers(others, Util::colorize(QString("%1 kicks %2.").arg(myName, enemyName), Teal), this, character);
                break;
        }

        if (character->hp() == 0) {
            character->die();
        }
    } else {
        switch (qrand() % 3) {
            case 0:
                character->send(Util::colorize(QString("%1 tries to punch you, but misses.").arg(myName), Green));
                send(Util::colorize(QString("You try to punch %1, but miss.").arg(enemyName), Teal));
                Util::sendOthers(others, Util::colorize(QString("%1 tries to punch %2, but misses.").arg(myName, enemyName), Teal), this, character);
                break;
            case 1:
                character->send(Util::colorize(QString("%1 tries to grab you, but you shake %2 off.").arg(myName, objectPronoun()), Green));
                send(Util::colorize(QString("You try to grab %1, but %2 shakes you off.").arg(enemyName, character->subjectPronoun()), Teal));
                Util::sendOthers(others, Util::colorize(QString("%1 tries to grab %2, but gets shaken off.").arg(myName, enemyName), Teal), this, character);
                break;
            case 2:
                character->send(Util::colorize(QString("%1 kicks at you, but fails to hurt you.").arg(myName), Green));
                send(Util::colorize(QString("You kick at %1, but fail to hurt %2.").arg(enemyName, character->objectPronoun()), Teal));
                Util::sendOthers(others, Util::colorize(QString("%1 kicks at %2, but fails to hurt %3.").arg(myName, enemyName, character->objectPronoun()), Teal), this, character);
                break;
        }
    }

    stun(4000 - (25 * stats().dexterity));
}

void Character::die() {

    Area *area = currentArea().cast<Area *>();
    GameObjectPtrList others = area->players();
    Util::sendOthers(others, Util::colorize(QString("%1 died.").arg(name()), Teal));
    if (inventory().length() > 0) {
        Util::sendOthers(others, Util::colorize(QString("%1 was carrying %2.").arg(name(), Util::joinItems(inventory())), Teal));
        foreach (const GameObjectPtr &item, inventory()) {
            area->addItem(item);
        }
    }

    area->removeNPC(this);
    setDeleted();
}

void Character::stun(int timeout) {

    if (m_stunTimeout) {
        killTimer(m_stunTimeout);
    }

    int seconds = timeout / 1000;
    int firstTimeout = timeout % 1000;
    if (firstTimeout == 0) {
        firstTimeout = 1000;
    } else {
        seconds++;
    }

    m_secondsStunned = seconds;
    m_stunTimeout = startTimer(firstTimeout);
    m_oddStunTimer = (firstTimeout < 1000);
}

void Character::setLeaveOnActive(bool leaveOnActive) {

    m_leaveOnActive = leaveOnActive;
}

void Character::timerEvent(QTimerEvent *event) {

    if (event->timerId() == m_stunTimeout) {
        m_secondsStunned--;

        if (m_secondsStunned > 0) {
            if (m_oddStunTimer) {
                killTimer(m_stunTimeout);
                m_stunTimeout = startTimer(1000);
                m_oddStunTimer = false;
            }
        } else {
            killTimer(m_stunTimeout);
            m_stunTimeout = 0;

            if (m_leaveOnActive) {
                leave(currentArea());
                m_leaveOnActive = false;
            } else {
                invokeTrigger("onactive");
            }
        }
    } else {
        Item::timerEvent(event);
    }
}
