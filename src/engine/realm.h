#ifndef REALM_H
#define REALM_H

#include <QHash>

#include "gameobject.h"


class Player;
class GameObjectSyncThread;

class Realm : public GameObject {

    Q_OBJECT

    public:
        static Realm *instance() { Q_ASSERT(s_instance); return s_instance; }

        static void instantiate();
        static void destroy();

        bool isInitialized() const { return m_initialized; }

        void registerObject(GameObject *gameObject);
        void unregisterObject(GameObject *gameObject);
        GameObject *getObject(const char *objectType, uint id) const;
        Q_INVOKABLE GameObject *getObject(const QString &objectType, uint id) const;

        void registerPlayer(Player *player);
        void unregisterPlayer(Player *player);
        Player *getPlayer(const QString &name) const;

        uint uniqueObjectId();

        void syncObject(GameObject *gameObject);

        static QString saveDirPath();
        static QString saveObjectPath(const char *objectType, uint id);

    private:
        static Realm *s_instance;

        bool m_initialized;

        uint m_nextId;
        QHash<uint, GameObject *> m_objectMap;
        QHash<QString, Player *> m_playerMap;

        GameObjectSyncThread *m_syncThread;

        explicit Realm();
        virtual ~Realm();
};

#endif // REALM_H
