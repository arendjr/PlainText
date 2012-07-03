#ifndef AREA_H
#define AREA_H

#include "gameobject.h"
#include "gameobjectptr.h"


class Area : public GameObject {

    Q_OBJECT

    public:
        Area(Realm *realm, uint id, Options options = NoOptions);
        virtual ~Area();

        const GameObjectPtrList &exits() const { return m_exits; }
        virtual void addExit(const GameObjectPtr &exit);
        virtual void removeExit(const GameObjectPtr &exit);
        virtual void setExits(const GameObjectPtrList &exits);
        Q_PROPERTY(GameObjectPtrList exits READ exits WRITE setExits)

        const GameObjectPtrList &players() const { return m_players; }
        virtual void addPlayer(const GameObjectPtr &player);
        virtual void removePlayer(const GameObjectPtr &player);
        virtual void setPlayers(const GameObjectPtrList &players);
        Q_PROPERTY(GameObjectPtrList players READ players WRITE setPlayers STORED false)

        const GameObjectPtrList &npcs() const { return m_npcs; }
        virtual void addNPC(const GameObjectPtr &npc);
        virtual void removeNPC(const GameObjectPtr &npc);
        virtual void setNPCs(const GameObjectPtrList &npcs);
        Q_PROPERTY(GameObjectPtrList npcs READ npcs WRITE setNPCs STORED false)

        const GameObjectPtrList &items() const { return m_items; }
        virtual void addItem(const GameObjectPtr &item);
        virtual void removeItem(const GameObjectPtr &item);
        virtual void setItems(const GameObjectPtrList &items);
        Q_PROPERTY(GameObjectPtrList items READ items WRITE setItems)

        GameObjectPtrList characters() const { return m_players + m_npcs; }
        Q_PROPERTY(GameObjectPtrList characters READ characters STORED false)

        GameObjectPtrList objects() const { return m_exits + characters() + m_items; }
        Q_PROPERTY(GameObjectPtrList objects READ objects STORED false)

    private:
        GameObjectPtrList m_exits;
        GameObjectPtrList m_players;
        GameObjectPtrList m_npcs;
        GameObjectPtrList m_items;
};

#endif // AREA_H
