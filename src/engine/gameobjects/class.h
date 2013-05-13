#ifndef CLASS_H
#define CLASS_H

#include "characterstats.h"
#include "gameobject.h"


class Class : public GameObject {

    Q_OBJECT

    public:
        Class(Realm *realm, int id = 0, Options options = NoOptions);
        virtual ~Class();

        const CharacterStats &stats() const { return m_stats; }
        void setStats(const CharacterStats &stats);
        Q_PROPERTY(CharacterStats stats READ stats WRITE setStats)

        const CharacterStats &statsSuggestion() const { return m_statsSuggestion; }
        void setStatsSuggestion(const CharacterStats &statsSuggestion);
        Q_PROPERTY(CharacterStats statsSuggestion READ statsSuggestion WRITE setStatsSuggestion)

    private:
        CharacterStats m_stats;
        CharacterStats m_statsSuggestion;
};

#endif // CLASS_H
