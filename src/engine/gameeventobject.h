#ifndef GAMEEVENTOBJECT_H
#define GAMEEVENTOBJECT_H

#include "gameevent.h"
#include "gameobject.h"


class Room;


class GameEventObject : public GameObject {

    Q_OBJECT

    public:
        GameEventObject(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~GameEventObject();

        GameEventType eventType() const { return m_eventType; }
        void setEventType(GameEventType eventType);
        Q_PROPERTY(GameEventType eventType READ eventType WRITE setEventType)

        const QString &description() const { return m_description; }
        void setDescription(const QString &description);
        Q_PROPERTY(QString description READ description WRITE setDescription)

        const QString &distantDescription() const { return m_distantDescription; }
        void setDistantDescription(const QString &distantDescription);
        Q_PROPERTY(QString distantDescription READ distantDescription WRITE setDistantDescription)

        const QString &veryDistantDescription() const { return m_veryDistantDescription; }
        void setVeryDistantDescription(const QString &veryDistantDescription);
        Q_PROPERTY(QString veryDistantDescription READ veryDistantDescription
                                                  WRITE setVeryDistantDescription)

        GameEvent *createInRoom(Room *origin, double strength,
                                const GameObjectPtrList &excludedCharacters = GameObjectPtrList());

    private:
        GameEventType m_eventType;
        QString m_description;
        QString m_distantDescription;
        QString m_veryDistantDescription;
};

#endif // GAMEEVENTOBJECT_H
