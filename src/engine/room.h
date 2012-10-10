#ifndef ROOM_H
#define ROOM_H

#include "gameobject.h"
#include "gameobjectptr.h"


class Room : public GameObject {

    Q_OBJECT

    public:
        Room(Realm *realm, uint id, Options options = NoOptions);
        virtual ~Room();

        const GameObjectPtrList &exits() const { return m_exits; }
        Q_INVOKABLE void addExit(const GameObjectPtr &exit);
        Q_INVOKABLE void removeExit(const GameObjectPtr &exit);
        void setExits(const GameObjectPtrList &exits);
        Q_PROPERTY(GameObjectPtrList exits READ exits WRITE setExits)

        const GameObjectPtrList &players() const { return m_players; }
        Q_INVOKABLE void addPlayer(const GameObjectPtr &player);
        Q_INVOKABLE void removePlayer(const GameObjectPtr &player);
        void setPlayers(const GameObjectPtrList &players);
        Q_PROPERTY(GameObjectPtrList players READ players WRITE setPlayers STORED false)

        const GameObjectPtrList &npcs() const { return m_npcs; }
        Q_INVOKABLE void addNPC(const GameObjectPtr &npc);
        Q_INVOKABLE void removeNPC(const GameObjectPtr &npc);
        void setNPCs(const GameObjectPtrList &npcs);
        Q_PROPERTY(GameObjectPtrList npcs READ npcs WRITE setNPCs STORED false)

        const GameObjectPtrList &items() const { return m_items; }
        Q_INVOKABLE void addItem(const GameObjectPtr &item);
        Q_INVOKABLE void removeItem(const GameObjectPtr &item);
        void setItems(const GameObjectPtrList &items);
        Q_PROPERTY(GameObjectPtrList items READ items WRITE setItems)

        Q_INVOKABLE void addGold(double amount);

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

#endif // ROOM_H
