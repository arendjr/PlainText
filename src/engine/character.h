#ifndef CHARACTER_H
#define CHARACTER_H

#include "item.h"
#include "gameobjectptr.h"

#include <QString>


class Character : public Item {

    Q_OBJECT

    public:
        explicit Character(uint id, Options options = NoOptions);
        virtual ~Character();

        virtual void setName(const QString &newName);

        const GameObjectPtr &currentArea() const { return m_currentArea; }
        virtual void setCurrentArea(const GameObjectPtr &currentArea);
        Q_PROPERTY(GameObjectPtr currentArea READ currentArea WRITE setCurrentArea)

        const GameObjectPtrList &inventory() const { return m_inventory; }
        Q_INVOKABLE virtual void addInventoryItem(const GameObjectPtr &item);
        Q_INVOKABLE virtual void removeInventoryItem(const GameObjectPtr &item);
        virtual void setInventory(const GameObjectPtrList &inventory);
        Q_PROPERTY(GameObjectPtrList inventory READ inventory WRITE setInventory)

        int hp() const { return m_hp; }
        virtual void adjustHp(int delta);
        virtual void setHp(int hp);
        Q_PROPERTY(int hp READ hp WRITE setHp)

        int mp() const { return m_mp; }
        virtual void adjustMp(int delta);
        virtual void setMp(int mp);
        Q_PROPERTY(int mp READ mp WRITE setMp)

        int sp() const { return m_sp; }
        virtual void adjustSp(int delta);
        virtual void setSp(int sp);
        Q_PROPERTY(int sp READ sp WRITE setSp)

        int gold() const { return m_gold; }
        virtual void adjustGold(int delta);
        virtual void setGold(int gold);
        Q_PROPERTY(int gold READ gold WRITE setGold)

        Q_INVOKABLE virtual void open(const GameObjectPtr &exit);
        Q_INVOKABLE virtual void close(const GameObjectPtr &exit);
        Q_INVOKABLE virtual void go(const GameObjectPtr &exit);

        virtual void enter(const GameObjectPtr &area);
        virtual void leave(const GameObjectPtr &area, const QString &exitName = QString());

        Q_INVOKABLE virtual void say(const QString &message);
        Q_INVOKABLE virtual void shout(const QString &message);
        Q_INVOKABLE virtual void talk(const GameObjectPtr &character, const QString &message);
        Q_INVOKABLE virtual void tell(const GameObjectPtr &player, const QString &message);

    protected:
        explicit Character(const char *objectType, uint id, Options options = NoOptions);

    private:
        GameObjectPtr m_currentArea;

        GameObjectPtrList m_inventory;

        int m_hp;
        int m_mp;
        int m_sp;
        int m_gold;
};

#endif // CHARACTER_H
