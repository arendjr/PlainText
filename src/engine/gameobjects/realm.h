#ifndef REALM_H
#define REALM_H

#include <QDateTime>
#include <QHash>
#include <QSet>
#include <QStringList>
#include <QVector>

#include "gameevent.h"
#include "gameobject.h"
#include "gameobjectptr.h"
#include "gameobjectsyncthread.h"
#include "gamethread.h"
#include "logthread.h"


class CommandInterpreter;
class CommandRegistry;
class Event;
class LogMessage;
class Player;
class ScriptEngine;
class TriggerRegistry;

class Realm : public GameObject {

    Q_OBJECT

    public:
        Realm(Options options = NoOptions);
        virtual ~Realm();

        static Realm *instance();

        virtual void init();
        bool isInitialized() const { return m_initialized; }

        void registerObject(GameObject *gameObject);
        void unregisterObject(GameObject *gameObject);
        GameObject *getObject(GameObjectType objectType, uint id);
        Q_INVOKABLE GameObject *getObject(const QString &objectType, uint id);
        Q_INVOKABLE GameObject *createObject(const QString &objectType);
        QVector<GameObject *> allObjects(GameObjectType objectType) const;

        Q_INVOKABLE GameObjectPtrList players() const;
        Q_INVOKABLE GameObjectPtrList onlinePlayers() const;
        void registerPlayer(Player *player);
        void unregisterPlayer(Player *player);
        Q_INVOKABLE GameObject *getPlayer(const QString &name) const;

        Q_INVOKABLE void addReservedName(const QString &name);
        Q_INVOKABLE QStringList reservedNames() const { return m_reservedNames; }

        const QDateTime &dateTime() const { return m_dateTime; }
        virtual void setDateTime(const QDateTime &dateTime);
        Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)

        Q_INVOKABLE GameObjectPtrList areas() const { return m_areas; }
        Q_INVOKABLE GameObjectPtrList rooms() const { return m_rooms; }
        Q_INVOKABLE GameObjectPtrList races() const { return m_races; }
        Q_INVOKABLE GameObjectPtrList classes() const { return m_classes; }

        Q_INVOKABLE GameEvent *createEvent(const QString &eventType, const GameObjectPtr &origin,
                                           double strength);

        uint uniqueObjectId();

        void enqueueEvent(Event *event);

        void addModifiedObject(GameObject *object);
        void enqueueModifiedObjects();

        void enqueueLogMessage(LogMessage *message);

        inline int startTimer(GameObject *object, int timeout) {
            return m_gameThread.startTimer(object, timeout);
        }

        inline int startInterval(GameObject *object, int interval) {
            return m_gameThread.startInterval(object, interval);
        }

        inline void stopTimer(int id) {
            m_gameThread.stopTimer(id);
        }

        inline void stopInterval(int id) {
            m_gameThread.stopInterval(id);
        }

        virtual void invokeTimer(int timerId);

        ScriptEngine *scriptEngine() const { return m_scriptEngine; }
        void setScriptEngine(ScriptEngine *scriptEngine);

        CommandRegistry *commandRegistry() const { return m_commandRegistry; }
        CommandInterpreter *commandInterpreter() const { return m_commandInterpreter; }

        TriggerRegistry *triggerRegistry() const { return m_triggerRegistry; }

    signals:
        void hourPassed(const QDateTime &dateTime);
        void dayPassed(const QDateTime &dateTime);

    private:
        bool m_initialized;

        uint m_nextId;
        QHash<uint, GameObject *> m_objectMap;
        QHash<QString, Player *> m_playerMap;

        QStringList m_reservedNames;

        GameObjectPtrList m_areas;
        GameObjectPtrList m_rooms;
        GameObjectPtrList m_races;
        GameObjectPtrList m_classes;
        int m_numObjects[GameObjectType::NumValues];

        QDateTime m_dateTime;
        int m_timeIntervalId;

        GameThread m_gameThread;

        GameObjectSyncThread m_syncThread;
        QSet<GameObject *> m_modifiedObjects;

        LogThread m_logThread;

        ScriptEngine *m_scriptEngine;

        CommandRegistry *m_commandRegistry;
        CommandInterpreter *m_commandInterpreter;

        TriggerRegistry *m_triggerRegistry;
};

#endif // REALM_H
