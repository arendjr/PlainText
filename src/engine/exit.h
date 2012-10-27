#ifndef EXIT_H
#define EXIT_H

#include "gameobject.h"
#include "gameobjectptr.h"


class Exit : public GameObject {

    Q_OBJECT

    public:
        Exit(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Exit();

        const GameObjectPtr &destination() const { return m_destination; }
        void setDestination(const GameObjectPtr &destination);
        Q_PROPERTY(GameObjectPtr destination READ destination WRITE setDestination)

        const GameObjectPtr &oppositeExit() const { return m_oppositeExit; }
        void setOppositeExit(const GameObjectPtr &oppositeExit);
        Q_PROPERTY(GameObjectPtr oppositeExit READ oppositeExit WRITE setOppositeExit)

        bool isDoor() const { return m_door; }
        void setDoor(bool door);
        Q_PROPERTY(bool door READ isDoor WRITE setDoor)

        bool isOpen() const { return m_open; }
        void setOpen(bool open);
        Q_PROPERTY(bool open READ isOpen WRITE setOpen STORED false)

        bool isHidden() const { return m_hidden; }
        void setHidden(bool hidden);
        Q_PROPERTY(bool hidden READ isHidden WRITE setHidden)

    private:
        GameObjectPtr m_destination;
        GameObjectPtr m_oppositeExit;
        bool m_door;
        bool m_open;
        bool m_hidden;
};

#endif // EXIT_H
