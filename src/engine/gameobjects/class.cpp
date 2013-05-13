#include "class.h"


#define super GameObject

Class::Class(Realm *realm, int id, Options options) :
    super(realm, GameObjectType::Class, id, (Options) (options | AutomaticNameForms)),
    m_stats(0) {

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

void Class::setStatsSuggestion(const CharacterStats &statsSuggestion) {

    if (m_statsSuggestion != statsSuggestion) {
        m_statsSuggestion = statsSuggestion;

        setModified();
    }
}
