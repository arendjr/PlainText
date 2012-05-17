#ifndef EXIT_H
#define EXIT_H

#include <QBasicTimer>

#include "gameobject.h"
#include "gameobjectptr.h"


class Exit : public GameObject {

    Q_OBJECT

    public:
        explicit Exit(uint id, Options options = NoOptions);
        virtual ~Exit();

        const GameObjectPtr &destinationArea() const { return m_destinationArea; }
        virtual void setDestinationArea(const GameObjectPtr &destinationArea);
        Q_PROPERTY(GameObjectPtr destinationArea READ destinationArea WRITE setDestinationArea)

        double size() const { return m_size; }
        virtual void setSize(double size);
        Q_PROPERTY(double size READ size WRITE setSize)

        bool isDoor() const { return m_door; }
        virtual void setDoor(bool door);
        Q_PROPERTY(bool door READ isDoor WRITE setDoor)

        bool isOpen() const { return m_open; }
        virtual void setOpen(bool open);
        Q_PROPERTY(bool open READ isOpen WRITE setOpen STORED false)

        bool isHidden() const { return m_hidden; }
        virtual void setHidden(bool hidden);
        Q_PROPERTY(bool hidden READ isHidden WRITE setHidden)

    protected:
        virtual void timerEvent(QTimerEvent *event);

    private:
        GameObjectPtr m_destinationArea;
        double m_size;
        bool m_door;
        bool m_open;
        bool m_hidden;

        QBasicTimer m_doorTimer;
};

#endif // EXIT_H
