#ifndef AREA_H
#define AREA_H

#include "gameobject.h"
#include "gameobjectptr.h"


class Area : public GameObject {

    Q_OBJECT

    public:
        Area(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Area();

        const GameObjectPtrList &rooms() const { return m_rooms; }
        Q_INVOKABLE void addRoom(const GameObjectPtr &room);
        Q_INVOKABLE void removeRoom(const GameObjectPtr &room);
        void setRooms(const GameObjectPtrList &rooms);
        Q_PROPERTY(GameObjectPtrList rooms READ rooms WRITE setRooms)

    private:
        GameObjectPtrList m_rooms;
};

#endif // AREA_H
