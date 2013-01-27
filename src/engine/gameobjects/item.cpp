#include "item.h"


#define super GameObject

Item::Item(Realm *realm, uint id, Options options) :
    Item(realm, GameObjectType::Item, id, options) {
}

Item::Item(Realm *realm, GameObjectType objectType, uint id, Options options) :
    super(realm, objectType, id, objectType == GameObjectType::Player ? options :
                                 (Options) (options | AutomaticNameForms)),
    m_position(0, 0, 0),
    m_weight(0),
    m_cost(0.0),
    m_flags(ItemFlags::NoFlags) {
}

Item::~Item() {
}

void Item::setPosition(const Point3D &position) {

    if (m_position != position) {
        m_position = position;

        setModified();
    }
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

void Item::setFlags(ItemFlags flags) {

    if (m_flags != flags) {
        m_flags = flags;

        setModified();
    }
}
