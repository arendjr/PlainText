#ifndef RACE_H
#define RACE_H

#include "characterstats.h"
#include "gameobject.h"
#include "gameobjectptr.h"


class Race : public GameObject {

    Q_OBJECT

    public:
        explicit Race(uint id, Options options = NoOptions);
        virtual ~Race();

        const CharacterStats &stats() const { return m_stats; }
        virtual void setStats(const CharacterStats &stats);
        Q_PROPERTY(CharacterStats stats READ stats WRITE setStats)

        const GameObjectPtrList &classes() const { return m_classes; }
        virtual void setClasses(const GameObjectPtrList &classes);
        Q_PROPERTY(GameObjectPtrList classes READ classes WRITE setClasses)

        const GameObjectPtr &startingArea() const { return m_startingArea; }
        virtual void setStartingArea(const GameObjectPtr &startingArea);
        Q_PROPERTY(GameObjectPtr startingArea READ startingArea WRITE setStartingArea)

    private:
        CharacterStats m_stats;

        GameObjectPtrList m_classes;
        GameObjectPtr m_startingArea;
};

#endif // RACE_H
