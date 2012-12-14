#ifndef CHARACTER_H
#define CHARACTER_H

#include <QString>

#include "effect.h"
#include "gameobjectptr.h"
#include "statsitem.h"
#include "vector3d.h"


class Character : public StatsItem {

    Q_OBJECT

    public:
        Character(Realm *realm, uint id = 0, Options options = NoOptions);
        Character(Realm *realm, GameObjectType objectType, uint id, Options options = NoOptions);
        virtual ~Character();

        int height() const { return m_height; }
        void setHeight(int height);
        Q_PROPERTY(int height READ height WRITE setHeight)

        const GameObjectPtr &currentRoom() const { return m_currentRoom; }
        void setCurrentRoom(const GameObjectPtr &currentRoom);
        Q_PROPERTY(GameObjectPtr currentRoom READ currentRoom WRITE setCurrentRoom)

        const Vector3D &direction() const { return m_direction; }
        void setDirection(const Vector3D &direction);
        Q_PROPERTY(Vector3D direction READ direction WRITE setDirection)

        const GameObjectPtrList &inventory() const { return m_inventory; }
        Q_INVOKABLE void addInventoryItem(const GameObjectPtr &item);
        Q_INVOKABLE void removeInventoryItem(const GameObjectPtr &item);
        void setInventory(const GameObjectPtrList &inventory);
        Q_PROPERTY(GameObjectPtrList inventory READ inventory WRITE setInventory)

        const GameObjectPtrList &sellableItems() const { return m_sellableItems; }
        Q_INVOKABLE void addSellableItem(const GameObjectPtr &item);
        Q_INVOKABLE void removeSellableItem(const GameObjectPtr &item);
        void setSellableItems(const GameObjectPtrList &items);
        Q_PROPERTY(GameObjectPtrList sellableItems READ sellableItems WRITE setSellableItems)

        const GameObjectPtr &race() const { return m_race; }
        void setRace(const GameObjectPtr &race);
        Q_PROPERTY(GameObjectPtr race READ race WRITE setRace)

        const GameObjectPtr &characterClass() const { return m_class; }
        void setClass(const GameObjectPtr &characterClass);
        Q_PROPERTY(GameObjectPtr characterClass READ characterClass WRITE setClass)

        const QString &gender() const { return m_gender; }
        void setGender(const QString &gender);
        Q_PROPERTY(QString gender READ gender WRITE setGender)

        QString subjectPronoun() const;
        Q_PROPERTY(QString subjectPronoun READ subjectPronoun STORED false)

        QString objectPronoun() const;
        Q_PROPERTY(QString objectPronoun READ objectPronoun STORED false)

        QString possessiveAdjective() const;
        Q_PROPERTY(QString possessiveAdjective READ possessiveAdjective STORED false)

        int respawnTime() const { return m_respawnTime; }
        void setRespawnTime(int respawnTime);
        Q_PROPERTY(int respawnTime READ respawnTime WRITE setRespawnTime)

        int respawnTimeVariation() const { return m_respawnTimeVariation; }
        void setRespawnTimeVariation(int respawnTimeVariation);
        Q_PROPERTY(int respawnTimeVariation READ respawnTimeVariation WRITE setRespawnTimeVariation)

        int hp() const { return m_hp; }
        void setHp(int hp);
        Q_PROPERTY(int hp READ hp WRITE setHp)

        int maxHp() const { return m_maxHp; }
        void setMaxHp(int maxHp);
        Q_PROPERTY(int maxHp READ maxHp WRITE setMaxHp)

        int mp() const { return m_mp; }
        void setMp(int mp);
        Q_PROPERTY(int mp READ mp WRITE setMp)

        int maxMp() const { return m_maxMp; }
        void setMaxMp(int maxMp);
        Q_PROPERTY(int maxMp READ maxMp WRITE setMaxMp)

        double gold() const { return m_gold; }
        void setGold(double gold);
        Q_PROPERTY(double gold READ gold WRITE setGold)

        const GameObjectPtr &weapon() const { return m_weapon; }
        void setWeapon(const GameObjectPtr &weapon);
        Q_PROPERTY(GameObjectPtr weapon READ weapon WRITE setWeapon)

        const GameObjectPtr &secondaryWeapon() const { return m_secondaryWeapon; }
        void setSecondaryWeapon(const GameObjectPtr &secondaryWeapon);
        Q_PROPERTY(GameObjectPtr secondaryWeapon READ secondaryWeapon WRITE setSecondaryWeapon)

        const GameObjectPtr &shield() const { return m_shield; }
        void setShield(const GameObjectPtr &shield);
        Q_PROPERTY(GameObjectPtr shield READ shield WRITE setShield)

        Q_INVOKABLE int inventoryWeight() const;

        Q_INVOKABLE int totalWeight() const;

        const GameObjectPtr &group() const { return m_group; }
        Q_PROPERTY(GameObjectPtr group READ group STORED false)

        EffectList effects() const { return m_effects; }
        Q_INVOKABLE void addEffect(const Effect &effect);
        Q_INVOKABLE void clearEffects();
        Q_INVOKABLE void clearNegativeEffects();
        Q_PROPERTY(EffectList effects READ effects STORED false)

        Q_INVOKABLE void go(const GameObjectPtr &pointer);

        Q_INVOKABLE void enter(const GameObjectPtr &roomPtr);
        Q_INVOKABLE void leave(const GameObjectPtr &roomPtr);

        Q_INVOKABLE void say(const QString &message);
        Q_INVOKABLE void shout(const QString &message);
        Q_INVOKABLE void talk(const GameObjectPtr &character, const QString &message);
        Q_INVOKABLE void tell(const GameObjectPtr &player, const QString &message);

        Q_INVOKABLE void follow(const GameObjectPtr &character);
        Q_INVOKABLE void lose(const GameObjectPtr &character = GameObjectPtr());
        Q_INVOKABLE void disband();

        Q_INVOKABLE void stun(int timeout);

        Q_INVOKABLE int secondsStunned() const { return m_secondsStunned; }

        Q_INVOKABLE void setLeaveOnActive(bool leaveOnActive);

        Q_INVOKABLE virtual void init();

        Q_INVOKABLE virtual GameObject *copy();

        Q_INVOKABLE virtual void invokeTimer(int timerId);
        Q_INVOKABLE virtual void killAllTimers();

    protected:
        virtual void changeName(const QString &newName);

        virtual void enteredRoom();

    private:
        int m_height;

        GameObjectPtr m_currentRoom;
        Vector3D m_direction;

        GameObjectPtrList m_inventory;
        GameObjectPtrList m_sellableItems;

        GameObjectPtr m_race;
        GameObjectPtr m_class;
        QString m_gender;

        int m_respawnTime;
        int m_respawnTimeVariation;

        int m_hp;
        int m_maxHp;

        int m_mp;
        int m_maxMp;

        double m_gold;

        GameObjectPtr m_weapon;
        GameObjectPtr m_secondaryWeapon;
        GameObjectPtr m_shield;

        GameObjectPtr m_group;

        EffectList m_effects;
        int m_effectsTimerId;

        int m_secondsStunned;
        int m_stunTimerId;
        bool m_leaveOnActive;

        int m_regenerationIntervalId;

        int updateEffects(qint64 now);
};

#endif // CHARACTER_H
