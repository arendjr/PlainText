#include "container.h"


#define super Item

Container::Container(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Container, id, options) {
}

Container::~Container() {
}

void Container::addItem(const GameObjectPtr &item) {

    if (!item->isItem()) {
        return;
    }

    if (!m_items.contains(item)) {
        m_items.append(item);

        setWeight(weight() + item.unsafeCast<Item *>()->weight());

        setModified();
    }
}

void Container::removeItem(const GameObjectPtr &item) {

    if (m_items.removeOne(item)) {
        setWeight(weight() - item.unsafeCast<Item *>()->weight());

        setModified();
    }
}

void Container::setItems(const GameObjectPtrList &items) {

    if (m_items != items) {
        m_items = items;

        if (mayReferenceOtherProperties()) {
            int weight = 0;
            for (const GameObjectPtr &item : items) {
                weight += item.cast<Item *>()->weight();
            }
            setWeight(weight);
        }

        setModified();
    }
}
