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

    private:
        CharacterStats m_stats;
};

#endif // CLASS_H
