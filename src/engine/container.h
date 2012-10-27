#ifndef CONTAINER_H
#define CONTAINER_H

#include "gameobjectptr.h"
#include "item.h"


class Container : public Item {

    Q_OBJECT

    public:
        Container(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Container();

        const GameObjectPtrList &items() const { return m_items; }
        Q_INVOKABLE void addItem(const GameObjectPtr &item);
        Q_INVOKABLE void removeItem(const GameObjectPtr &item);
        void setItems(const GameObjectPtrList &items);
        Q_PROPERTY(GameObjectPtrList items READ items WRITE setItems)

    private:
        GameObjectPtrList m_items;
};

#endif // CONTAINER_H
