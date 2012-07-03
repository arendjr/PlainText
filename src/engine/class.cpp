#include "class.h"


Class::Class(Realm *realm, int id, Options options) :
    GameObject(realm, "class", id, options) {

    setAutoDelete(false);
}

Class::~Class() {
}

void Class::setStats(const CharacterStats &stats) {

    if (m_stats != stats) {
        m_stats = stats;

        setModified();
    }
}
