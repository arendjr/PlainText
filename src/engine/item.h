#ifndef ITEM_H
#define ITEM_H

#include "gameobject.h"
#include "gameobjectptr.h"


class Item : public GameObject {

    Q_OBJECT

    public:
        Item(Realm *realm, uint id = 0, Options options = NoOptions);
        Item(Realm *realm, GameObjectType objectType, uint id, Options options = NoOptions);
        virtual ~Item();

        bool isPortable() const { return m_portable; }
        void setPortable(bool portable);
        Q_PROPERTY(bool portable READ isPortable WRITE setPortable)

        int weight() const { return m_weight; }
        Q_INVOKABLE void adjustWeight(int delta);
        void setWeight(int weight);
        Q_PROPERTY(int weight READ weight WRITE setWeight)

        double cost() const { return m_cost; }
        void setCost(double cost);
        Q_PROPERTY(double cost READ cost WRITE setCost)

        static Item *createGold(Realm *realm, double amount);

    private:
        bool m_portable;

        int m_weight;

        double m_cost;
};

#endif // ITEM_H
