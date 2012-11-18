#ifndef ROOM_H
#define ROOM_H

#include "gameobject.h"
#include "gameobjectptr.h"
#include "point3d.h"


class Room : public GameObject {

    Q_OBJECT

    public:
        Room(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Room();

        const Point3D &position() const { return m_position; }
        void setPosition(const Point3D &position);
        Q_PROPERTY(Point3D position READ position WRITE setPosition)

        GameObjectPtrList exits() const { return m_exits; }
        Q_INVOKABLE void addExit(const GameObjectPtr &exit);
        Q_INVOKABLE void removeExit(const GameObjectPtr &exit);
        void setExits(const GameObjectPtrList &exits);
        Q_PROPERTY(GameObjectPtrList exits READ exits WRITE setExits)

        const GameObjectPtrList &portals() const { return m_portals; }
        Q_INVOKABLE void addPortal(const GameObjectPtr &portal);
        Q_INVOKABLE void removePortal(const GameObjectPtr &portal);
        void setPortals(const GameObjectPtrList &portals);
        Q_PROPERTY(GameObjectPtrList portals READ portals WRITE setPortals)

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

        GameObjectPtrList characters() const { return m_players + m_npcs; }
        Q_PROPERTY(GameObjectPtrList characters READ characters STORED false)

        GameObjectPtrList objects() const { return m_exits + characters() + m_items; }
        Q_PROPERTY(GameObjectPtrList objects READ objects STORED false)

    private:
        Point3D m_position;
        GameObjectPtrList m_exits;
        GameObjectPtrList m_portals;
        GameObjectPtrList m_players;
        GameObjectPtrList m_npcs;
        GameObjectPtrList m_items;
};

#endif // ROOM_H
