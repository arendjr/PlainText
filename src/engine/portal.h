#ifndef PORTAL_H
#define PORTAL_H

#include "gameobject.h"
#include "gameobjectptr.h"


class Portal : public GameObject {

    Q_OBJECT

    public:
        Portal(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Portal();

        const QString &name2() const { return m_name2; }
        void setName2(const QString &name2);
        Q_PROPERTY(QString name2 READ name2 WRITE setName2)

        const QString &description2() const { return m_description2; }
        void setDescription2(const QString &description2);
        Q_PROPERTY(QString description2 READ description2 WRITE setDescription2)

        const GameObjectPtr &room() const { return m_room; }
        void setRoom(const GameObjectPtr &room);
        Q_PROPERTY(GameObjectPtr room READ room WRITE setRoom)

        const GameObjectPtr &room2() const { return m_room2; }
        void setRoom2(const GameObjectPtr &room2);
        Q_PROPERTY(GameObjectPtr room2 READ room2 WRITE setRoom2)

        Q_INVOKABLE QString nameFromRoom(const GameObjectPtr &room);
        Q_INVOKABLE QString descriptionFromRoom(const GameObjectPtr &room);

    private:
        QString m_name2;
        QString m_description2;
        GameObjectPtr m_room;
        GameObjectPtr m_room2;
};

#endif // PORTAL_H
