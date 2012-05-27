#include "class.h"


Class::Class(uint id, Options options) :
    GameObject("class", id, options) {

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
