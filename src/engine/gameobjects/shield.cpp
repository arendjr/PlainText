#include "shield.h"


#define super StatsItem

Shield::Shield(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Shield, id, options) {

    setFlags(ItemFlags::Portable);
}

Shield::~Shield() {
}

void Shield::setCategory(const QString &category) {

    if (m_category != category) {
        m_category = category;

        setModified();
    }
}
