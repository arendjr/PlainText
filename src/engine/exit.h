#ifndef EXIT_H
#define EXIT_H

#include "gameobject.h"
#include "gameobjectptr.h"


class Exit : public GameObject {

    Q_OBJECT

    public:
        Exit(Realm *realm, uint id, Options options = NoOptions);
        virtual ~Exit();

        const GameObjectPtr &destinationArea() const { return m_destinationArea; }
        virtual void setDestinationArea(const GameObjectPtr &destinationArea);
        Q_PROPERTY(GameObjectPtr destinationArea READ destinationArea WRITE setDestinationArea)

        const GameObjectPtr &oppositeExit() const { return m_oppositeExit; }
        virtual void setOppositeExit(const GameObjectPtr &oppositeExit);
        Q_PROPERTY(GameObjectPtr oppositeExit READ oppositeExit WRITE setOppositeExit)

        bool isDoor() const { return m_door; }
        virtual void setDoor(bool door);
        Q_PROPERTY(bool door READ isDoor WRITE setDoor)

        bool isOpen() const { return m_open; }
        virtual void setOpen(bool open);
        Q_PROPERTY(bool open READ isOpen WRITE setOpen STORED false)

        bool isHidden() const { return m_hidden; }
        virtual void setHidden(bool hidden);
        Q_PROPERTY(bool hidden READ isHidden WRITE setHidden)

    private:
        GameObjectPtr m_destinationArea;
        GameObjectPtr m_oppositeExit;
        bool m_door;
        bool m_open;
        bool m_hidden;
};

#endif // EXIT_H
