#include "shield.h"


Shield::Shield(Realm *realm, uint id, Options options) :
    StatsItem(realm, "shield", id, options) {

    setPortable(true);
}

Shield::~Shield() {
}

void Shield::setDeflectMessages(const CombatMessageList &deflectMessages) {

    if (m_deflectMessages != deflectMessages) {
        m_deflectMessages = deflectMessages;

        setModified();
    }
}
