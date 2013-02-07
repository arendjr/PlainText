#include "character.h"

#include <QDateTime>
#include <QDebug>

#include "commandinterpreter.h"
#include "group.h"
#include "logutil.h"
#include "movementsoundevent.h"
#include "movementvisualevent.h"
#include "player.h"
#include "portal.h"
#include "race.h"
#include "realm.h"
#include "room.h"
#include "shield.h"
#include "speechevent.h"
#include "util.h"
#include "weapon.h"


#define NO_STUN \
    if (m_secondsStunned > 0) { \
        send(QString("Please wait %1 seconds.").arg(m_secondsStunned), Olive); \
        return; \
    }


#define super StatsItem

Character::Character(Realm *realm, uint id, Options options) :
    Character(realm, GameObjectType::Character, id, options) {

    if (~options & Copy) {
        m_regenerationIntervalId = realm->startInterval(this, 45000);
    }
}

Character::Character(Realm *realm, GameObjectType objectType, uint id, Options options) :
    super(realm, objectType, id, options),
    m_height(0),
    m_direction(0, 0, 0),
    m_gender("male"),
    m_respawnTime(0),
    m_respawnTimeVariation(0),
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

void Character::setCurrentRoom(const GameObjectPtr &currentRoom) {

    if (m_currentRoom != currentRoom) {
        m_currentRoom = currentRoom;

        setModified();
    }
}

void Character::setDirection(const Vector3D &direction) {

    if (m_direction != direction) {
        m_direction = direction;

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

        setModified();
    }
}

QString Character::subjectPronoun() const {

    return m_race.isNull() || m_race->name() != "animal" ?
           m_gender == "male" ?
           "he" : "she" : "it";
}

QString Character::objectPronoun() const {

    return m_race.isNull() || m_race->name() != "animal" ?
           m_gender == "male" ?
           "him" : "her" : "it";
}

QString Character::possessiveAdjective() const {

    return m_race.isNull() || m_race->name() != "animal" ?
           m_gender == "male" ?
                    "his" : "her" : "its";
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

void Character::enter(const GameObjectPtr &roomPtr) {

    try {
        setCurrentRoom(roomPtr);

        Room *room = roomPtr.cast<Room *>();
        room->addCharacter(this);

        enteredRoom();

        for (const GameObjectPtr &character : room->characters()) {
            if (character != this) {
                character->invokeTrigger("oncharacterentered", this);
            }
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::enter(): " << exception.what();
    }
}

void Character::leave(const GameObjectPtr &roomPtr) {

    try {
        Room *room = roomPtr.cast<Room *>();
        room->removeCharacter(this);
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::leave(): " << exception.what();
    }
}

void Character::follow(const GameObjectPtr &characterPtr) {

    try {
        Character *character = characterPtr.cast<Character *>();

        if (character == this) {
            send("You cannot follow yourself.");
            return;
        }
        if (isPlayer()) {
            if (!characterPtr->isPlayer()) {
                GameObjectPtrList characters = currentRoom().cast<Room *>()->characters();
                QString name = character->definiteName(characters, DefiniteArticles);
                send(QString("You cannot follow %1.").arg(name));
                return;
            }
        } else {
            if (characterPtr->isPlayer()) {
                return;
            }
        }

        if (!m_group.isNull()) {
            Group *group = m_group.cast<Group *>();
            if (group->leader() == characterPtr) {
                send(QString("You are already following %1.").arg(character->name()));
                return;
            }
            if (group->members().contains(characterPtr)) {
                send(QString("You are already in the same group as %1.").arg(character->name()));
                return;
            }

            lose();
        }

        Group *group;
        if (character->m_group.isNull()) {
            group = new Group(realm());
            group->setLeader(characterPtr);
            character->m_group = group;
        } else {
            group = character->m_group.cast<Group *>();
        }

        if (group->members().length() == 0) {
            send(QString("You started following %1.").arg(character->name()));
            character->send(QString("%1 started following you.").arg(name()));
        } else {
            send(QString("You joined the group of %1, led by %2.").arg(group->members().joinFancy(),
                                                                       group->leader()->name()));
            group->send(QString("%1 joined your group.").arg(name()));
        }

        group->addMember(this);
        m_group = group;
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::follow(): " << exception.what();
    }
}

void Character::lose(const GameObjectPtr &character) {

    try {
        if (m_group.isNull()) {
            send("You're not part of any group.");
            return;
        }

        Group *group = m_group.cast<Group *>();

        if (character.isNull()) {
            if (group->leader() == this) {
                disband();
            } else {
                if (group->members().length() > 1) {
                    group->removeMember(this);
                    group->send(QString("%1 left the group.").arg(name()));
                    send("You left the group.");
                } else {
                    Character *leader = group->leader().cast<Character *>();
                    leader->send(QString("%1 has stopped following you.").arg(name()));
                    leader->m_group = GameObjectPtr();
                    group->setDeleted();
                    send(QString("You stopped following %1.").arg(group->leader()->name()));
                }
            }
            m_group = GameObjectPtr();
        } else {
            if (character == this) {
                lose();
            } else {
                if (group->leader() == this) {
                    if (group->members().contains(character)) {
                        if (group->members().length() > 1) {
                            group->removeMember(character);
                            character->send(QString("%1 has removed you from the group.").arg(name()));
                            send(QString("You removed %1 from the group.").arg(character->name()));
                        } else {
                            disband();
                        }
                    } else {
                        send(QString("%1 is not a member of the group.").arg(character->name()));
                    }
                } else {
                    send("Only the group leader can lose people from the group.");
                }
            }
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::lose(): " << exception.what();
    }
}

void Character::disband() {

    try {
        if (m_group.isNull()) {
            send("You're not part of any group.");
            return;
        }

        Group *group = m_group.cast<Group *>();
        if (group->leader() != this) {
            send("Only the group leader can disband the group.");
            return;
        }

        for (const GameObjectPtr &memberPtr : group->members()) {
            Character *member = memberPtr.cast<Character *>();
            member->send(QString("%1 has disbanded the group.").arg(name()));
            member->m_group = GameObjectPtr();
        }

        group->setDeleted();
        send("You disbanded the group.");
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::disband(): " << exception.what();
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

void Character::execute(const QString &command) {

    realm()->commandInterpreter()->execute(this, command);
}

void Character::setLeaveOnActive(bool leaveOnActive) {

    m_leaveOnActive = leaveOnActive;
}

void Character::init() {

    try {
        // guarantee our current room lists us, otherwise we may end up in a
        // real limbo if we died and a server termination killed the respawn
        // timers. in addition, this allows Room::characters to become a
        // non-stored property, saving many disk writes when characters walk
        // around
        if (!isPlayer() && !currentRoom().isNull()) {
            Room *room = currentRoom().cast<Room *>();
            room->addCharacter(this);
        }

        super::init();

        invokeTrigger("onspawn");
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::init(): " << exception.what();
    }
}

GameObject *Character::copy() {

    Character *other = qobject_cast<Character *>(super::copy());
    if (!m_weapon.isNull()) {
        other->setWeapon(m_weapon->copy());
    }
    if (!m_secondaryWeapon.isNull()) {
        other->setSecondaryWeapon(m_secondaryWeapon->copy());
    }
    if (!m_shield.isNull()) {
        other->setShield(m_shield->copy());
    }
    other->setInventory(GameObjectPtrList());
    for (const GameObjectPtr &item : m_inventory) {
        other->addInventoryItem(item->copy());
    }
    return other;
}

void Character::invokeTimer(int timerId) {

    if (timerId == m_effectsTimerId) {
        int nextTimeout = updateEffects(QDateTime::currentMSecsSinceEpoch());
        m_effectsTimerId = (nextTimeout > -1 ? realm()->startTimer(this, nextTimeout) : 0);
    } else if (timerId == m_stunTimerId) {
        m_secondsStunned--;

        if (m_secondsStunned > 0) {
            m_stunTimerId = realm()->startTimer(this, 1000);
        } else {
            m_stunTimerId = 0;

            if (m_leaveOnActive) {
                leave(currentRoom());
                m_leaveOnActive = false;
            } else {
                invokeTrigger("onactive");
            }
        }
    } else if (timerId == m_regenerationIntervalId) {
        invokeScriptMethod("regenerate");
    } else {
        super::invokeTimer(timerId);
    }
}

void Character::killAllTimers() {

    super::killAllTimers();

    clearEffects();

    m_secondsStunned = 0;
    if (m_stunTimerId) {
        realm()->stopTimer(m_stunTimerId);
        m_stunTimerId = 0;
    }
}

void Character::changeName(const QString &newName) {

    super::changeName(newName);

    if (newName.toLower() != newName) {
        setIndefiniteArticle("");
    }

    realm()->addReservedName(newName);
}

void Character::enteredRoom() {

    invokeTrigger("onentered");
    invokeScriptMethod("enteredRoom");
}

int Character::updateEffects(qint64 now) {

    int nextTimeout = -1;
    for (int i = 0; i < m_effects.length(); i++) {
        Effect &effect = m_effects[i];

        int msecsLeft = (effect.started + effect.delay) - now;
        while (msecsLeft <= 0) {
            if (effect.hpDelta != 0) {
                m_hp = qBound(0, m_hp + effect.hpDelta, m_maxHp);
            }
            if (effect.mpDelta != 0) {
                m_mp = qBound(0, m_mp + effect.mpDelta, m_maxMp);
            }
            setModified();
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
