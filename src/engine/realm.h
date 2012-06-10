#ifndef REALM_H
#define REALM_H

#include <QDateTime>
#include <QHash>

#include "gameobject.h"
#include "gameobjectptr.h"


class Player;
class GameObjectSyncThread;

class Realm : public GameObject {

    Q_OBJECT

    public:
        static Realm *instance() { Q_ASSERT(s_instance); return s_instance; }

        static void instantiate();
        static void destroy();

        explicit Realm(Options options = NoOptions);
        virtual ~Realm();

        bool isInitialized() const { return m_initialized; }

        void registerObject(GameObject *gameObject);
        void unregisterObject(GameObject *gameObject);
        GameObject *getObject(const char *objectType, uint id) const;
        Q_INVOKABLE GameObject *getObject(const QString &objectType, uint id) const;

        Q_INVOKABLE GameObjectPtrList players() const;
        Q_INVOKABLE GameObjectPtrList onlinePlayers() const;
        void registerPlayer(Player *player);
        void unregisterPlayer(Player *player);
        Player *getPlayer(const QString &name) const;

        const GameObjectPtrList &races() const { return m_races; }

        const QDateTime &dateTime() const { return m_dateTime; }
        virtual void setDateTime(const QDateTime &dateTime);
        Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)

        uint uniqueObjectId();

        void syncObject(GameObject *gameObject);

        static QString saveDirPath();
        static QString saveObjectPath(const char *objectType, uint id);

    signals:
        void hourPassed(const QDateTime &dateTime);
        void dayPassed(const QDateTime &dateTime);

    protected:
        virtual void timerEvent(QTimerEvent *event);

    private:
        static Realm *s_instance;

        bool m_initialized;

        uint m_nextId;
        QHash<uint, GameObject *> m_objectMap;
        QHash<QString, Player *> m_playerMap;

        GameObjectPtrList m_races;

        QDateTime m_dateTime;
        int m_timeTimer;

        GameObjectSyncThread *m_syncThread;
};

#endif // REALM_H
