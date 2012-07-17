#ifndef CHARACTER_H
#define CHARACTER_H

#include <QString>

#include "effect.h"
#include "gameobjectptr.h"
#include "statsitem.h"


class Character : public StatsItem {

    Q_OBJECT

    public:
        Character(Realm *realm, uint id, Options options = NoOptions);
        Character(Realm *realm, const char *objectType, uint id, Options options = NoOptions);
        virtual ~Character();

        virtual void setName(const QString &newName);

        Q_INVOKABLE virtual int totalWeight() const;

        virtual void setStats(const CharacterStats &stats);
        Q_INVOKABLE virtual CharacterStats totalStats() const;

        int height() const { return m_height; }
        virtual void setHeight(int height);
        Q_PROPERTY(int height READ height WRITE setHeight)

        const GameObjectPtr &currentArea() const { return m_currentArea; }
        virtual void setCurrentArea(const GameObjectPtr &currentArea);
        Q_PROPERTY(GameObjectPtr currentArea READ currentArea WRITE setCurrentArea)

        const GameObjectPtrList &inventory() const { return m_inventory; }
        Q_INVOKABLE virtual void addInventoryItem(const GameObjectPtr &item);
        Q_INVOKABLE virtual void removeInventoryItem(const GameObjectPtr &item);
        virtual void setInventory(const GameObjectPtrList &inventory);
        Q_PROPERTY(GameObjectPtrList inventory READ inventory WRITE setInventory)

        const GameObjectPtrList &sellableItems() const { return m_sellableItems; }
        Q_INVOKABLE virtual void addSellableItem(const GameObjectPtr &item);
        Q_INVOKABLE virtual void removeSellableItem(const GameObjectPtr &item);
        virtual void setSellableItems(const GameObjectPtrList &items);
        Q_PROPERTY(GameObjectPtrList sellableItems READ sellableItems WRITE setSellableItems)

        const GameObjectPtr &race() const { return m_race; }
        virtual void setRace(const GameObjectPtr &race);
        Q_PROPERTY(GameObjectPtr race READ race WRITE setRace)

        const GameObjectPtr &characterClass() const { return m_class; }
        virtual void setClass(const GameObjectPtr &characterClass);
        Q_PROPERTY(GameObjectPtr characterClass READ characterClass WRITE setClass)

        const QString &gender() const { return m_gender; }
        virtual void setGender(const QString &gender);
        Q_PROPERTY(QString gender READ gender WRITE setGender)

        const QString &subjectPronoun() const { return m_subjectPronoun; }
        Q_PROPERTY(QString subjectPronoun READ subjectPronoun STORED false)

        const QString &objectPronoun() const { return m_objectPronoun; }
        Q_PROPERTY(QString objectPronoun READ objectPronoun STORED false)

        const QString &possessiveAdjective() const { return m_possessiveAdjective; }
        Q_PROPERTY(QString possessiveAdjective READ possessiveAdjective STORED false)

        int respawnTime() const { return m_respawnTime; }
        virtual void setRespawnTime(int respawnTime);
        Q_PROPERTY(int respawnTime READ respawnTime WRITE setRespawnTime)

        int respawnTimeVariation() const { return m_respawnTimeVariation; }
        virtual void setRespawnTimeVariation(int respawnTimeVariation);
        Q_PROPERTY(int respawnTimeVariation READ respawnTimeVariation WRITE setRespawnTimeVariation)

        int hp() const { return m_hp; }
        virtual void adjustHp(int delta);
        virtual void setHp(int hp);
        Q_PROPERTY(int hp READ hp WRITE setHp)

        int maxHp() const { return m_maxHp; }
        virtual void setMaxHp(int maxHp);
        Q_PROPERTY(int maxHp READ maxHp WRITE setMaxHp)

        int mp() const { return m_mp; }
        virtual void adjustMp(int delta);
        virtual void setMp(int mp);
        Q_PROPERTY(int mp READ mp WRITE setMp)

        int maxMp() const { return m_maxMp; }
        virtual void setMaxMp(int maxMp);
        Q_PROPERTY(int maxMp READ maxMp WRITE setMaxMp)

        double gold() const { return m_gold; }
        virtual void adjustGold(double delta);
        virtual void setGold(double gold);
        Q_PROPERTY(double gold READ gold WRITE setGold)

        const GameObjectPtr &weapon() const { return m_weapon; }
        virtual void setWeapon(const GameObjectPtr &weapon);
        Q_PROPERTY(GameObjectPtr weapon READ weapon WRITE setWeapon)

        const GameObjectPtr &secondaryWeapon() const { return m_secondaryWeapon; }
        virtual void setSecondaryWeapon(const GameObjectPtr &secondaryWeapon);
        Q_PROPERTY(GameObjectPtr secondaryWeapon READ secondaryWeapon WRITE setSecondaryWeapon)

        const GameObjectPtr &shield() const { return m_shield; }
        virtual void setShield(const GameObjectPtr &shield);
        Q_PROPERTY(GameObjectPtr shield READ shield WRITE setShield)

        EffectList effects() const { return m_effects; }
        Q_INVOKABLE void addEffect(const Effect &effect);
        Q_INVOKABLE void clearEffects();
        Q_INVOKABLE void clearNegativeEffects();
        Q_PROPERTY(EffectList effects READ effects STORED false)

        Q_INVOKABLE virtual void open(const GameObjectPtr &exit);
        Q_INVOKABLE virtual void close(const GameObjectPtr &exit);
        Q_INVOKABLE virtual void go(const GameObjectPtr &exit);

        virtual void enter(const GameObjectPtr &area);
        virtual void leave(const GameObjectPtr &area, const QString &exitName = QString());

        Q_INVOKABLE virtual void say(const QString &message);
        Q_INVOKABLE virtual void shout(const QString &message);
        Q_INVOKABLE virtual void talk(const GameObjectPtr &character, const QString &message);
        Q_INVOKABLE virtual void tell(const GameObjectPtr &player, const QString &message);

        Q_INVOKABLE virtual void kill(const GameObjectPtr &character);
        Q_INVOKABLE virtual void die(const GameObjectPtr &attacker = GameObjectPtr());

        int secondsStunned() const { return m_secondsStunned; }
        Q_INVOKABLE virtual void stun(int timeout);
        void setLeaveOnActive(bool leaveOnActive);

        virtual void init();

        virtual void invokeTimer(int timerId);

    protected:
        virtual void killAllTimers();

    private:
        int m_height;

        GameObjectPtr m_currentArea;

        GameObjectPtrList m_inventory;
        GameObjectPtrList m_sellableItems;

        GameObjectPtr m_race;
        GameObjectPtr m_class;
        QString m_gender;
        QString m_subjectPronoun;
        QString m_objectPronoun;
        QString m_possessiveAdjective;

        int m_respawnTime;
        int m_respawnTimeVariation;
        int m_respawnTimerId;

        int m_hp;
        int m_maxHp;

        int m_mp;
        int m_maxMp;

        double m_gold;

        GameObjectPtr m_weapon;
        GameObjectPtr m_secondaryWeapon;
        GameObjectPtr m_shield;

        EffectList m_effects;
        int m_effectsTimerId;

        int m_secondsStunned;
        int m_stunTimerId;
        bool m_leaveOnActive;

        int m_regenerationIntervalId;

        int updateEffects(qint64 now);
};

#endif // CHARACTER_H
