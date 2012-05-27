#ifndef CLASS_H
#define CLASS_H

#include "characterstats.h"
#include "gameobject.h"


class Class : public GameObject {

    Q_OBJECT

    public:
        explicit Class(uint id, Options options = NoOptions);
        virtual ~Class();

        const CharacterStats &stats() const { return m_stats; }
        virtual void setStats(const CharacterStats &stats);
        Q_PROPERTY(CharacterStats stats READ stats WRITE setStats)

    private:
        CharacterStats m_stats;
};

#endif // CLASS_H
