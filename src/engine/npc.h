#ifndef NPC_H
#define NPC_H

#include "item.h"
#include "gameobjectptr.h"

#include <QString>


class NPC : public Item {

    Q_OBJECT

    public:
        explicit NPC(uint id, Options options = NoOptions);
        virtual ~NPC();

        virtual void setName(const QString &newName);

        const GameObjectPtr &currentArea() const { return m_currentArea; }
        virtual void setCurrentArea(const GameObjectPtr &currentArea);
        Q_PROPERTY(GameObjectPtr currentArea READ currentArea WRITE setCurrentArea)

        const GameObjectPtrList &inventory() const { return m_inventory; }
        virtual void addInventoryItem(const GameObjectPtr &item);
        virtual void removeInventoryItem(const GameObjectPtr &item);
        virtual void setInventory(const GameObjectPtrList &inventory);
        Q_PROPERTY(GameObjectPtrList inventory READ inventory WRITE setInventory)

        int hp() const { return m_hp; }
        virtual void setHp(int hp);
        Q_PROPERTY(int hp READ hp WRITE setHp)

        void enter(const GameObjectPtr &area);
        void leave(const GameObjectPtr &area, const QString &exitName = QString());

    private:
        GameObjectPtr m_currentArea;

        GameObjectPtrList m_inventory;

        int m_hp;
};

#endif // NPC_H
