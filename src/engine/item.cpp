#include "item.h"


#define super GameObject

Item::Item(Realm *realm, uint id, Options options) :
    Item(realm, "item", id, options) {
}

Item::Item(Realm *realm, const char *objectType, uint id, Options options) :
    super(realm, objectType, id, options),
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

Item *Item::createGold(Realm *realm, double amount) {

    Item *gold = new Item(realm);
    gold->setName(QString("$%1 worth of gold").arg(amount));
    gold->setCost(amount);
    gold->setPortable(true);
    return gold;
}
