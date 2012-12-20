#include "item.h"


#define super GameObject

Item::Item(Realm *realm, uint id, Options options) :
    Item(realm, GameObjectType::Item, id, options) {
}

Item::Item(Realm *realm, GameObjectType objectType, uint id, Options options) :
    super(realm, objectType, id, objectType == GameObjectType::Player ? options :
                                 (Options) (options | AutomaticNameForms)),
    m_portable(false),
    m_weight(0),
    m_cost(0.0) {
}

Item::~Item() {
}

void Item::setPortable(bool portable) {

    if (m_portable != portable) {
        m_portable = portable;

        setModified();
    }
}

void Item::adjustWeight(int delta) {

    setWeight(m_weight + delta);
}

void Item::setWeight(int weight) {

    if (m_weight != weight) {
        m_weight = weight;

        setModified();
    }
}

void Item::setCost(double cost) {

    if (m_cost != cost) {
        m_cost = cost;

        setModified();
    }
}
