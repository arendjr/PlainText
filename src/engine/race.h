#ifndef RACE_H
#define RACE_H

#include "characterstats.h"
#include "combatmessage.h"
#include "gameobject.h"
#include "gameobjectptr.h"


class Race : public GameObject {

    Q_OBJECT

    public:
        Race(Realm *realm, uint id, Options options = NoOptions);
        virtual ~Race();

        const QString &adjective() const { return m_adjective; }
        virtual void setAdjective(const QString &adjective);
        Q_PROPERTY(QString adjective READ adjective WRITE setAdjective)

        const CharacterStats &stats() const { return m_stats; }
        virtual void setStats(const CharacterStats &stats);
        Q_PROPERTY(CharacterStats stats READ stats WRITE setStats)

        int height() const { return m_height; }
        virtual void setHeight(int height);
        Q_PROPERTY(int height READ height WRITE setHeight)

        int weight() const { return m_weight; }
        virtual void setWeight(int weight);
        Q_PROPERTY(int weight READ weight WRITE setWeight)

        const GameObjectPtrList &classes() const { return m_classes; }
        virtual void setClasses(const GameObjectPtrList &classes);
        Q_PROPERTY(GameObjectPtrList classes READ classes WRITE setClasses)

        const GameObjectPtr &startingArea() const { return m_startingArea; }
        virtual void setStartingArea(const GameObjectPtr &startingArea);
        Q_PROPERTY(GameObjectPtr startingArea READ startingArea WRITE setStartingArea)

        const CombatMessageList &hitMessages() const { return m_hitMessages; }
        virtual void setHitMessages(const CombatMessageList &hitMessages);
        Q_PROPERTY(CombatMessageList hitMessages READ hitMessages WRITE setHitMessages)

        const CombatMessageList &missMessages() const { return m_missMessages; }
        virtual void setMissMessages(const CombatMessageList &missMessages);
        Q_PROPERTY(CombatMessageList missMessages READ missMessages WRITE setMissMessages)

        bool playerSelectable() const { return m_playerSelectable; }
        virtual void setPlayerSelectable(bool playerSelectable);
        Q_PROPERTY(bool playerSelectable READ playerSelectable WRITE setPlayerSelectable)

    private:
        QString m_adjective;

        CharacterStats m_stats;
        int m_height;
        int m_weight;

        GameObjectPtrList m_classes;
        GameObjectPtr m_startingArea;

        CombatMessageList m_hitMessages;
        CombatMessageList m_missMessages;

        bool m_playerSelectable;
};

#endif // RACE_H
