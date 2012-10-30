#ifndef RACE_H
#define RACE_H

#include "characterstats.h"
#include "gameobject.h"
#include "gameobjectptr.h"


class Race : public GameObject {

    Q_OBJECT

    public:
        Race(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Race();

        const QString &adjective() const { return m_adjective; }
        void setAdjective(const QString &adjective);
        Q_PROPERTY(QString adjective READ adjective WRITE setAdjective)

        const CharacterStats &stats() const { return m_stats; }
        void setStats(const CharacterStats &stats);
        Q_PROPERTY(CharacterStats stats READ stats WRITE setStats)

        int height() const { return m_height; }
        void setHeight(int height);
        Q_PROPERTY(int height READ height WRITE setHeight)

        int weight() const { return m_weight; }
        void setWeight(int weight);
        Q_PROPERTY(int weight READ weight WRITE setWeight)

        const GameObjectPtrList &classes() const { return m_classes; }
        void setClasses(const GameObjectPtrList &classes);
        Q_PROPERTY(GameObjectPtrList classes READ classes WRITE setClasses)

        const GameObjectPtr &startingRoom() const { return m_startingRoom; }
        void setStartingRoom(const GameObjectPtr &startingRoom);
        Q_PROPERTY(GameObjectPtr startingRoom READ startingRoom WRITE setStartingRoom)

        bool playerSelectable() const { return m_playerSelectable; }
        void setPlayerSelectable(bool playerSelectable);
        Q_PROPERTY(bool playerSelectable READ playerSelectable WRITE setPlayerSelectable)

    private:
        QString m_adjective;

        CharacterStats m_stats;
        int m_height;
        int m_weight;

        GameObjectPtrList m_classes;
        GameObjectPtr m_startingRoom;

        bool m_playerSelectable;
};

#endif // RACE_H
