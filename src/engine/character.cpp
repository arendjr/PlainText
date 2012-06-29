#include "character.h"

#include <cstdlib>

#include <QDateTime>
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
    m_respawnTime(0),
    m_respawnTimeVariation(0),
    m_respawnTimerId(0),
    m_hp(1),
    m_maxHp(1),
    m_mp(0),
    m_maxMp(0),
    m_gold(0.0),
    m_effectsTimerId(0),
    m_effectsTimerStarted(0),
    m_modifiersTimerId(0),
    m_modifiersTimerStarted(0),
    m_secondsStunned(0),
    m_stunTimerId(0),
    m_oddStunTimer(false),
    m_leaveOnActive(false),
    m_regenerationTimerId(0) {

    setAutoDelete(false);

    m_regenerationTimerId = startTimer(45000);
}

Character::Character(const char *objectType, uint id, Options options) :
    Item(objectType, id, options),
    m_gender("male"),
    m_subjectPronoun("he"),
    m_objectPronoun("him"),
    m_respawnTime(0),
    m_respawnTimeVariation(0),
    m_respawnTimerId(0),
    m_hp(1),
    m_maxHp(1),
    m_mp(0),
    m_maxMp(0),
    m_gold(0.0),
    m_effectsTimerId(0),
    m_effectsTimerStarted(0),
    m_modifiersTimerId(0),
    m_modifiersTimerStarted(0),
    m_secondsStunned(0),
    m_stunTimerId(0),
    m_oddStunTimer(false),
    m_leaveOnActive(false),
    m_regenerationTimerId(0) {

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

        adjustWeight(item.cast<Item *>()->weight());

        setModified();
    }
}

void Character::removeInventoryItem(const GameObjectPtr &item) {

    if (m_inventory.removeAll(item) > 0) {
        adjustWeight(-item.cast<Item *>()->weight());

        setModified();
    }
}

void Character::setInventory(const GameObjectPtrList &inventory) {

    if (m_inventory != inventory) {
        m_inventory = inventory;

        if (~options() & Copy) {
            int weight = m_stats.weight;
            foreach (const GameObjectPtr &item, m_inventory) {
                weight += item.cast<Item *>()->weight();
            }
            setWeight(weight);
        }

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

    if (m_sellableItems.removeAll(item) > 0) {
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
        m_subjectPronoun = (m_gender == "male" ? "he" : "she");
        m_objectPronoun = (m_gender == "male" ? "him" : "her");

        setModified();
    }
}

void Character::setStats(const CharacterStats &stats) {

    if (m_stats != stats) {
        m_stats = stats;

        if (~options() & Copy) {
            startBulkModification();

            setMaxHp(2 * m_stats.vitality);
            setMaxMp(m_stats.intelligence);

            int weight = m_stats.weight;
            foreach (const GameObjectPtr &item, m_inventory) {
                weight += item.cast<Item *>()->weight();
            }
            setWeight(weight);

            commitBulkModification();
        }
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

void Character::adjustGold(double delta) {

    setGold(m_gold + delta);
}

void Character::setGold(double gold) {

    if (m_gold != gold) {
        m_gold = qMax(gold, 0.0);

        setModified();
    }
}

void Character::addEffect(const Effect &effect) {

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    int nextTimeout = updateEffects(now);

    if (nextTimeout == -1 || effect.delay < nextTimeout) {
        if (m_effectsTimerId) {
            killTimer(m_effectsTimerId);
        }
        m_effectsTimerId = startTimer(effect.delay);
        m_effectsTimerStarted = now;
    }

    m_effects.append(effect);
}

void Character::clearEffects() {

    m_effects.clear();
    if (m_effectsTimerId) {
        killTimer(m_effectsTimerId);
        m_effectsTimerId = 0;
    }
    m_effectsTimerStarted = 0;
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

void Character::addModifier(const Modifier &modifier) {

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    int nextTimeout = updateModifiers(now);

    if (nextTimeout == -1 || modifier.duration < nextTimeout) {
        if (m_modifiersTimerId) {
            killTimer(m_modifiersTimerId);
        }
        m_modifiersTimerId = startTimer(modifier.duration);
        m_modifiersTimerStarted = now;
    }

    m_modifiers.append(modifier);
}

void Character::clearModifiers() {

    m_modifiers.clear();
    if (m_modifiersTimerId) {
        killTimer(m_modifiersTimerId);
        m_modifiersTimerId = 0;
    }
    m_modifiersTimerStarted = 0;
}

void Character::clearNegativeModifiers() {

    for (int i = 0; i < m_modifiers.length(); i++) {
        Modifier &modifier = m_modifiers[i];
        CharacterStats &stats = modifier.stats;

        if ((stats.strength + stats.dexterity + stats.vitality +
             stats.endurance + stats.intelligence + stats.faith) < 0)  {
            m_modifiers.removeAt(i);
            i--;
            continue;
        }
    }

    if (m_modifiers.length() == 0) {
        clearModifiers();
    }
}

void Character::open(const GameObjectPtr &exitPtr) {

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

        QString text = QString("%1 opens the %2.").arg(name(), exit->name());
        Util::sendOthers(currentArea().cast<Area *>()->players(), text, this);
    }
}

void Character::close(const GameObjectPtr &exitPtr) {

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
        character->invokeTrigger("oncharacterentered", this);
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
                        QString("%1 %2 left to the %3.").arg(Util::capitalize(article), name(),
                                                             exitName));
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

        players << adjacentArea->players();
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
    foreach (const GameObjectPtr &other, others) {
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
            inventoryStr = Util::joinItems(inventory());
        }
        if (m_gold > 0.0) {
            if (!inventoryStr.isEmpty()) {
                inventoryStr += " and ";
            }
            inventoryStr += QString("$%1 worth of gold").arg(m_gold);
        }

        players.send(QString("%1 was carrying %2.").arg(myName, inventoryStr), Teal);

        foreach (const GameObjectPtr &item, inventory()) {
            area->addItem(item);
        }
    }

    GameObjectPtrList others = area->characters();
    others.removeOne(this);
    foreach (const GameObjectPtr &other, others) {
        other->invokeTrigger("oncharacterdied", this, attacker);
    }

    area->removeNPC(this);

    if (m_respawnTime) {
        setInventory(GameObjectPtrList());

        killAllTimers();
        clearEffects();
        clearModifiers();

        int respawnTime = m_respawnTime;
        if (m_respawnTimeVariation) {
            respawnTime += qrand() % m_respawnTimeVariation;
        }
        m_respawnTimerId = startTimer(respawnTime);
    } else {
        setDeleted();
    }
}

void Character::stun(int timeout) {

    if (m_stunTimerId) {
        killTimer(m_stunTimerId);
    }

    int seconds = timeout / 1000;
    int firstTimeout = timeout % 1000;
    if (firstTimeout == 0) {
        firstTimeout = 1000;
    } else {
        seconds++;
    }

    m_secondsStunned = seconds;
    m_stunTimerId = startTimer(firstTimeout);
    m_oddStunTimer = (firstTimeout < 1000);
}

void Character::setLeaveOnActive(bool leaveOnActive) {

    m_leaveOnActive = leaveOnActive;
}

void Character::init() {

    // guarantee our current area lists us, otherwise we may end up in a real
    // limbo if we died and a server termination killed the respawn timers.
    // in addition, this allows Area::npcs to become a non-stored property,
    // saving many disk writes when NPCs walk around
    if (strcmp(objectType(), "character") == 0) {
        Area *area = currentArea().cast<Area *>();
        area->addNPC(this);
    }

    GameObject::init();
}

void Character::timerEvent(QTimerEvent *event) {

    if (event->timerId() == m_respawnTimerId) {
        killTimer(m_respawnTimerId);
        m_respawnTimerId = 0;

        m_hp = m_maxHp;
        m_mp = m_maxMp;

        setModified();

        enter(currentArea());

        invokeTrigger("onspawn");
    } else if (event->timerId() == m_effectsTimerId) {
        killTimer(m_effectsTimerId);
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        int nextTimeout = updateEffects(now);
        if (nextTimeout > -1) {
            m_effectsTimerId = startTimer(nextTimeout);
            m_effectsTimerStarted = now;
        } else {
            m_effectsTimerId = 0;
        }
    } else if (event->timerId() == m_modifiersTimerId) {
        killTimer(m_modifiersTimerId);
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        int nextTimeout = updateModifiers(now);
        if (nextTimeout > -1) {
            m_modifiersTimerId = startTimer(nextTimeout);
            m_modifiersTimerStarted = now;
        } else {
            m_modifiersTimerId = 0;
        }
    } else if (event->timerId() == m_stunTimerId) {
        m_secondsStunned--;

        if (m_secondsStunned > 0) {
            if (m_oddStunTimer) {
                killTimer(m_stunTimerId);
                m_stunTimerId = startTimer(1000);
                m_oddStunTimer = false;
            }
        } else {
            killTimer(m_stunTimerId);
            m_stunTimerId = 0;

            if (m_leaveOnActive) {
                leave(currentArea());
                m_leaveOnActive = false;
            } else {
                invokeTrigger("onactive");
            }
        }
    } else if (event->timerId() == m_regenerationTimerId) {
        adjustHp(qMax(stats().vitality / 15, 1));
    } else {
        Item::timerEvent(event);
    }
}

int Character::updateEffects(qint64 now) {

    Q_ASSERT(m_effects.length() ? (bool) m_effectsTimerStarted : true);

    int nextTimeout = -1;
    qint64 msecsPassed = now - m_effectsTimerStarted;
    for (int i = 0; i < m_effects.length(); i++) {
        Effect &effect = m_effects[i];

        effect.delay -= msecsPassed;
        if (effect.delay <= 0) {
            adjustHp(effect.hpDelta);
            adjustMp(effect.mpDelta);
            send(effect.message);

            effect.numOccurrences--;
            if (effect.numOccurrences <= 0) {
                m_effects.removeAt(i);
                i--;
                continue;
            }
        }

        if (nextTimeout == -1 || effect.delay < nextTimeout) {
            nextTimeout = effect.delay;
        }
    }

    return nextTimeout;
}

int Character::updateModifiers(qint64 now) {

    Q_ASSERT(m_modifiers.length() ? (bool) m_modifiersTimerStarted : true);

    int nextTimeout = -1;
    qint64 msecsPassed = now - m_modifiersTimerStarted;
    for (int i = 0; i < m_modifiers.length(); i++) {
        Modifier &modifier = m_modifiers[i];

        modifier.duration -= msecsPassed;
        if (modifier.duration <= 0) {
            m_modifiers.removeAt(i);
            i--;
            continue;
        }

        if (nextTimeout == -1 || modifier.duration < nextTimeout) {
            nextTimeout = modifier.duration;
        }
    }

    return nextTimeout;
}
