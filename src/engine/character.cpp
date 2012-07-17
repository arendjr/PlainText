#include "character.h"

#include <QDateTime>

#include "area.h"
#include "exit.h"
#include "player.h"
#include "realm.h"
#include "shield.h"
#include "util.h"
#include "weapon.h"


#define NOT_NULL(pointer) \
    if (pointer.isNull()) { \
        return; \
    }

#define NO_STUN \
    if (m_secondsStunned > 0) { \
        send(Util::colorize(QString("Please wait %1 seconds.").arg(m_secondsStunned), Olive)); \
        return; \
    }


Character::Character(Realm *realm, uint id, Options options) :
    Character(realm, "character", id, options) {

    if (~options & Copy) {
        m_regenerationIntervalId = realm->startInterval(this, 45000);
    }
}

Character::Character(Realm *realm, const char *objectType, uint id, Options options) :
    StatsItem(realm, objectType, id, options),
    m_gender("male"),
    m_subjectPronoun("he"),
    m_objectPronoun("him"),
    m_possessiveAdjective("his"),
    m_respawnTime(0),
    m_respawnTimeVariation(0),
    m_respawnTimerId(0),
    m_hp(1),
    m_maxHp(1),
    m_mp(0),
    m_maxMp(0),
    m_gold(0.0),
    m_effectsTimerId(0),
    m_secondsStunned(0),
    m_stunTimerId(0),
    m_leaveOnActive(false),
    m_regenerationIntervalId(0) {

    setAutoDelete(false);
}

Character::~Character() {

    if (m_regenerationIntervalId) {
        realm()->stopInterval(m_regenerationIntervalId);
    }
}

void Character::setName(const QString &newName) {

    StatsItem::setName(newName);

    if (newName.toLower() != newName) {
        setIndefiniteArticle("");
    }
}

int Character::totalWeight() const {

    int totalWeight = weight();
    for (const GameObjectPtr &item : m_inventory) {
        totalWeight += item.cast<Item *>()->weight();
    }
    return totalWeight;
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

    if (m_inventory.removeOne(item)) {
        setModified();
    }
}

void Character::setInventory(const GameObjectPtrList &inventory) {

    if (m_inventory != inventory) {
        m_inventory = inventory;

        setModified();
    }
}

void Character::addSellableItem(const GameObjectPtr &item) {

    if (!m_sellableItems.contains(item)) {
        m_sellableItems << item;

        setModified();
    }
}

void Character::removeSellableItem(const GameObjectPtr &item) {

    if (m_sellableItems.removeOne(item)) {
        setModified();
    }
}

void Character::setSellableItems(const GameObjectPtrList &items) {

    if (m_sellableItems != items) {
        m_sellableItems = items;

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

        if (m_gender == "male") {
            m_subjectPronoun = "he";
            m_objectPronoun = "him";
            m_possessiveAdjective = "his";
        } else {
            m_subjectPronoun = "she";
            m_objectPronoun = "her";
            m_possessiveAdjective = "her";
        }

        setModified();
    }
}

void Character::setStats(const CharacterStats &stats) {

    if (Character::stats() != stats) {
        StatsItem::setStats(stats);

        setMaxHp(2 * stats.vitality);
        setMaxMp(stats.intelligence);
    }
}

CharacterStats Character::totalStats() const {

    CharacterStats totalStats = StatsItem::totalStats();
    if (!m_weapon.isNull()) {
        totalStats += m_weapon.cast<Weapon *>()->totalStats();
    }
    if (!m_secondaryWeapon.isNull()) {
        totalStats += m_secondaryWeapon.cast<Weapon *>()->totalStats();
    }
    if (!m_shield.isNull()) {
        totalStats += m_shield.cast<Shield *>()->totalStats();
    }
    return totalStats;
}

void Character::setHeight(int height) {

    if (m_height != height) {
        m_height = height;

        setModified();
    }
}

void Character::setRespawnTime(int respawnTime) {

    if (m_respawnTime != respawnTime) {
        m_respawnTime = qMax(respawnTime, 0);

        setModified();
    }
}

void Character::setRespawnTimeVariation(int respawnTimeVariation) {

    if (m_respawnTimeVariation != respawnTimeVariation) {
        m_respawnTimeVariation = qMax(respawnTimeVariation, 0);

        setModified();
    }
}

void Character::adjustHp(int delta) {

    setHp(m_hp + delta);
}

void Character::setHp(int hp) {

    if (m_hp != hp) {
        if (mayReferenceOtherProperties()) {
            m_hp = qBound(0, hp, maxHp());
        } else {
            m_hp = hp;
        }

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
        if (mayReferenceOtherProperties()) {
            m_mp = qBound(0, mp, maxMp());
        } else {
            m_mp = mp;
        }

        setModified();
    }
}

void Character::setMaxMp(int maxMp) {

    if (m_maxMp != maxMp) {
        m_maxMp = qMax(maxMp, 0);

        setModified();
    }
}

void Character::adjustGold(double delta) {

    setGold(m_gold + delta);
}

void Character::setGold(double gold) {

    if (m_gold != gold) {
        m_gold = qMax(gold, 0.0);

        setModified();
    }
}

void Character::setWeapon(const GameObjectPtr &weapon) {

    if (m_weapon != weapon) {
        m_weapon = weapon;

        setModified();
    }
}

void Character::setSecondaryWeapon(const GameObjectPtr &secondaryWeapon) {

    if (m_secondaryWeapon != secondaryWeapon) {
        m_secondaryWeapon = secondaryWeapon;

        setModified();
    }
}

void Character::setShield(const GameObjectPtr &shield) {

    if (m_shield != shield) {
        m_shield = shield;

        setModified();
    }
}

void Character::addEffect(const Effect &effect) {

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    int nextTimeout = updateEffects(now);

    if (nextTimeout == -1 || effect.delay < nextTimeout) {
        if (m_effectsTimerId) {
            realm()->stopTimer(m_effectsTimerId);
        }
        m_effectsTimerId = realm()->startTimer(this, effect.delay);
    }

    m_effects.append(effect);
    m_effects.last().started = now;
}

void Character::clearEffects() {

    m_effects.clear();
    if (m_effectsTimerId) {
        realm()->stopTimer(m_effectsTimerId);
        m_effectsTimerId = 0;
    }
}

void Character::clearNegativeEffects() {

    for (int i = 0; i < m_effects.length(); i++) {
        Effect &effect = m_effects[i];

        if (effect.hpDelta < 0 || effect.mpDelta < 0) {
            m_effects.removeAt(i);
            i--;
            continue;
        }
    }

    if (m_effects.length() == 0) {
        clearEffects();
    }
}

void Character::open(const GameObjectPtr &exitPtr) {

    NOT_NULL(exitPtr)
    NO_STUN

    Exit *exit = exitPtr.cast<Exit *>();

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

        GameObjectPtrList others = currentArea().cast<Area *>()->players();
        others.removeOne(this);
        others.send(QString("%1 opens the %2.").arg(name(), exit->name()));
    }
}

void Character::close(const GameObjectPtr &exitPtr) {

    NOT_NULL(exitPtr)
    NO_STUN

    Exit *exit = exitPtr.cast<Exit *>();

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

        GameObjectPtrList others = currentArea().cast<Area *>()->players();
        others.removeOne(this);
        others.send(QString("%1 closes the %2.").arg(name(), exit->name()));
    } else {
        send(QString("The %1 is already closed.").arg(exit->name()));
    }
}

void Character::go(const GameObjectPtr &exitPtr) {

    NOT_NULL(exitPtr)
    NO_STUN

    Exit *exit = exitPtr.cast<Exit *>();

    if (exit->isDoor() && !exit->isOpen()) {
        send(QString("The %1 is closed.").arg(exit->name()));
        return;
    }

    Area *area = currentArea().cast<Area *>();
    for (const GameObjectPtr &character : area->npcs()) {
        if (!character->invokeTrigger("onexit", this, exit->name())) {
            return;
        }
    }

    if (!exit->invokeTrigger("onenter", this)) {
        return;
    }

    leave(currentArea(), exit->name());
    enter(exit->destinationArea());
}

void Character::enter(const GameObjectPtr &areaPtr) {

    Area *area = areaPtr.cast<Area *>();

    setCurrentArea(area);

    area->addNPC(this);

    area->players().send(QString("%1 arrived.").arg(indefiniteName(Capitalized)));

    for (const GameObjectPtr &character : area->npcs()) {
        character->invokeTrigger("oncharacterentered", this);
    }
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
            for (const GameObjectPtr &other : area->npcs()) {
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
                        QString("%1 %2 left to the %3.").arg(Util::capitalize(article), name(),
                                                             exitName));
            }
        }
        area->players().send(text);
    }
}

void Character::say(const QString &message) {

    QString text;
    if (message.endsWith(".") || message.endsWith("?") || message.endsWith("!")) {
        text = QString("%1 says, \"%2\"").arg(name(), Util::capitalize(message));
    } else {
        text = QString("%1 says, \"%2.\"").arg(name(), Util::capitalize(message));
    }
    currentArea().cast<Area *>()->players().send(text);
}

void Character::shout(const QString &message) {

    Area *area = currentArea().cast<Area *>();

    GameObjectPtrList players = area->players();
    players.send(QString("%1 shouts, \"%2\".").arg(definiteName(area->characters(),
                                                                Capitalized), message));

    GameObjectPtrList adjacentPlayers;
    for (const GameObjectPtr &exitPtr : area->exits()) {
        Exit *exit = exitPtr.cast<Exit *>();
        Area *adjacentArea = exit->destinationArea().cast<Area *>();

        adjacentPlayers << adjacentArea->players();
    }
    adjacentPlayers.send(QString("You hear %1 shouting, \"%2\".").arg(indefiniteName(), message));
}

void Character::talk(const GameObjectPtr &characterPtr, const QString &message) {

    NOT_NULL(characterPtr)

    Character *character = characterPtr.cast<Character *>();
    Player *player = qobject_cast<Player *>(character);
    if (player) {
        tell(player, message);
    } else {
        character->invokeTrigger("ontalk", this, message);
    }
}

void Character::tell(const GameObjectPtr &playerPtr, const QString &message) {

    NOT_NULL(playerPtr)

    Player *player = playerPtr.cast<Player *>();

    if (message.isEmpty()) {
        send(QString("Tell %1 what?").arg(player->name()));
        return;
    }

    player->send(QString("%1 tells you, \"%2\".").arg(name(), message));
    send(QString("You tell %1, \"%2\".").arg(player->name(), message));
}

void Character::kill(const GameObjectPtr &characterPtr) {

    NOT_NULL(characterPtr)
    NO_STUN

    Character *character = characterPtr.cast<Character *>();

    if (!character->invokeTrigger("onattack", this)) {
        return;
    }

    Area *area = currentArea().cast<Area *>();
    GameObjectPtrList others = area->players();
    others.removeOne(this);
    others.removeOne(characterPtr);

    QString myName = definiteName(area->characters(), Capitalized);
    QString enemyName = character->definiteName(area->characters());

    qreal hitChance = 100 * ((80 + stats().dexterity) / 160.0) *
                            ((100 - character->stats().dexterity) / 100.0);
    if (qrand() % 100 < hitChance) {
        int damage = qrand() % (int) (20.0 * (stats().strength / 40.0) *
                                             ((80 - character->stats().endurance) / 80.0)) + 1;

        character->adjustHp(-damage);

        switch (qrand() % 3) {
            case 0:
                character->send(QString("%1 deals you a sweeping punch, causing %2 damage.")
                                .arg(myName).arg(damage), Maroon);
                send(QString("You deal a sweeping punch to %1, causing %2 damage.")
                     .arg(enemyName).arg(damage), Teal);
                others.send(QString("%1 deals a sweeping punch to %2.").arg(myName, enemyName),
                            Teal);
                break;
            case 1:
                character->send(QString("%1 hits you on the jaw for %2 damage.")
                                .arg(myName).arg(damage), Maroon);
                send(QString("You hit %1 on the jaw for %2 damage.").arg(enemyName).arg(damage),
                     Teal);
                others.send(QString("%1 hits %2 on the jaw.").arg(myName, enemyName), Teal);
                break;
            case 2:
                character->send(QString("%1 kicks you for %2 damage.").arg(myName).arg(damage),
                                Maroon);
                send(QString("You kick %1 for %2 damage.").arg(enemyName).arg(damage), Teal);
                others.send(QString("%1 kicks %2.").arg(myName, enemyName), Teal);
                break;
        }

        if (character->hp() == 0) {
            character->die(this);
        }
    } else {
        switch (qrand() % 3) {
            case 0:
                character->send(QString("%1 tries to punch you, but misses.").arg(myName), Green);
                send(QString("You try to punch %1, but miss.").arg(enemyName), Teal);
                others.send(QString("%1 tries to punch %2, but misses.")
                                 .arg(myName, enemyName), Teal);
                break;
            case 1:
                character->send(QString("%1 tries to grab you, but you shake %2 off.")
                                .arg(myName, objectPronoun()), Green);
                send(QString("You try to grab %1, but %2 shakes you off.")
                     .arg(enemyName, character->subjectPronoun()), Teal);
                others.send(QString("%1 tries to grab %2, but gets shaken off.")
                            .arg(myName, enemyName), Teal);
                break;
            case 2:
                character->send(QString("%1 kicks at you, but fails to hurt you.")
                                .arg(myName), Green);
                send(QString("You kick at %1, but fail to hurt %2.")
                     .arg(enemyName, character->objectPronoun()), Teal);
                others.send(QString("%1 kicks at %2, but fails to hurt %3.")
                            .arg(myName, enemyName, character->objectPronoun()), Teal);
                break;
        }
    }

    stun(4000 - (25 * stats().dexterity));

    others = currentArea().cast<Area *>()->characters();
    others.removeOne(this);
    others.removeOne(characterPtr);
    for (const GameObjectPtr &other : others) {
        other->invokeTrigger("oncharacterattacked", this, characterPtr);
    }
}

void Character::die(const GameObjectPtr &attacker) {

    if (!invokeTrigger("ondie", attacker)) {
        return;
    }

    Area *area = currentArea().cast<Area *>();

    GameObjectPtrList players = area->players();

    QString myName = definiteName(area->characters(), Capitalized);
    players.send(QString("%1 died.").arg(myName), Teal);

    if (inventory().length() > 0 || m_gold > 0.0) {
        QString inventoryStr;
        if (inventory().length() > 0) {
            inventoryStr = inventory().joinFancy();
        }
        if (m_gold > 0.0) {
            if (!inventoryStr.isEmpty()) {
                inventoryStr += " and ";
            }
            inventoryStr += QString("$%1 worth of gold").arg(m_gold);
        }

        players.send(QString("%1 was carrying %2.").arg(myName, inventoryStr), Teal);

        for (const GameObjectPtr &item : inventory()) {
            area->addItem(item);
        }
    }

    GameObjectPtrList others = area->characters();
    others.removeOne(this);
    for (const GameObjectPtr &other : others) {
        other->invokeTrigger("oncharacterdied", this, attacker);
    }

    area->removeNPC(this);

    if (m_respawnTime) {
        setInventory(GameObjectPtrList());

        killAllTimers();

        int respawnTime = m_respawnTime;
        if (m_respawnTimeVariation) {
            respawnTime += qrand() % m_respawnTimeVariation;
        }
        m_respawnTimerId = realm()->startTimer(this, respawnTime);
    } else {
        setDeleted();
    }
}

void Character::stun(int timeout) {

    if (m_stunTimerId) {
        realm()->stopTimer(m_stunTimerId);
    }

    int seconds = timeout / 1000;
    int firstTimeout = timeout % 1000;
    if (firstTimeout == 0) {
        firstTimeout = 1000;
    } else {
        seconds++;
    }

    m_secondsStunned = seconds;
    m_stunTimerId = realm()->startTimer(this, firstTimeout);
}

void Character::setLeaveOnActive(bool leaveOnActive) {

    m_leaveOnActive = leaveOnActive;
}

void Character::init() {

    // guarantee our current area lists us, otherwise we may end up in a real
    // limbo if we died and a server termination killed the respawn timers.
    // in addition, this allows Area::npcs to become a non-stored property,
    // saving many disk writes when NPCs walk around
    if (!isPlayer()) {
        Area *area = currentArea().cast<Area *>();
        area->addNPC(this);
    }

    GameObject::init();
}

void Character::invokeTimer(int timerId) {

    if (timerId == m_respawnTimerId) {
        m_respawnTimerId = 0;

        m_hp = m_maxHp;
        m_mp = m_maxMp;

        setModified();

        enter(currentArea());

        invokeTrigger("onspawn");
    } else if (timerId == m_effectsTimerId) {
        int nextTimeout = updateEffects(QDateTime::currentMSecsSinceEpoch());
        m_effectsTimerId = (nextTimeout > -1 ? realm()->startTimer(this, nextTimeout) : 0);
    } else if (timerId == m_stunTimerId) {
        m_secondsStunned--;

        if (m_secondsStunned > 0) {
            m_stunTimerId = realm()->startTimer(this, 1000);
        } else {
            m_stunTimerId = 0;

            if (m_leaveOnActive) {
                leave(currentArea());
                m_leaveOnActive = false;
            } else {
                invokeTrigger("onactive");
            }
        }
    } else if (timerId == m_regenerationIntervalId) {
        adjustHp(qMax(stats().vitality / 15, 1));
    } else {
        StatsItem::invokeTimer(timerId);
    }
}

void Character::killAllTimers() {

    StatsItem::killAllTimers();

    if (m_respawnTimerId) {
        realm()->stopTimer(m_respawnTimerId);
        m_respawnTimerId = 0;
    }

    clearEffects();

    if (m_stunTimerId) {
        realm()->stopTimer(m_stunTimerId);
        m_stunTimerId = 0;
    }
}

int Character::updateEffects(qint64 now) {

    int nextTimeout = -1;
    for (int i = 0; i < m_effects.length(); i++) {
        Effect &effect = m_effects[i];

        int msecsLeft = (effect.started + effect.delay) - now;
        while (msecsLeft <= 0) {
            adjustHp(effect.hpDelta);
            adjustMp(effect.mpDelta);
            send(effect.message);

            effect.numOccurrences--;
            if (effect.numOccurrences > 0) {
                effect.started = now + msecsLeft;
                msecsLeft = effect.delay + msecsLeft;
            } else {
                m_effects.removeAt(i);
                i--;
                break;
            }
        }

        if (msecsLeft > 0 && (nextTimeout == -1 || msecsLeft < nextTimeout)) {
            nextTimeout = msecsLeft;
        }
    }

    return nextTimeout;
}
