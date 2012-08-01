#include "shield.h"


#define super StatsItem

Shield::Shield(Realm *realm, uint id, Options options) :
    super(realm, "shield", id, options) {

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
