#ifndef REALM_H
#define REALM_H

#include <QDateTime>
#include <QHash>
#include <QStringList>

#include "gameobject.h"
#include "gameobjectptr.h"


class CommandInterpreter;
class CommandRegistry;
class Event;
class LogMessage;
class Player;
class ScriptEngine;

class Realm : public GameObject {

    Q_OBJECT

    public:
        Realm(Options options = NoOptions);
        virtual ~Realm();

        static Realm *instance();

        void init();
        bool isInitialized() const { return m_initialized; }

        void registerObject(GameObject *gameObject);
        void unregisterObject(GameObject *gameObject);
        GameObject *getObject(GameObjectType objectType, uint id);
        GameObjectPtrList allObjects(GameObjectType objectType) const;

        uint uniqueObjectId();

    signals:
        void hourPassed(const QDateTime &dateTime);
        void dayPassed(const QDateTime &dateTime);

    private:
        bool m_initialized;

        uint m_nextId;
        QHash<uint, GameObject *> m_objectMap;
};

#endif // REALM_H
