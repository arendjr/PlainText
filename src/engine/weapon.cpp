#include "weapon.h"


#define super StatsItem

Weapon::Weapon(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Weapon, id, options) {

    setPortable(true);
}

Weapon::~Weapon() {
}

void Weapon::setCategory(const QString &category) {

    if (m_category != category) {
        m_category = category;

        setModified();
    }
}
