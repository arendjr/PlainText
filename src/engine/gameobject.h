#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QHash>
#include <QList>
#include <QMetaProperty>
#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>

#include "constants.h"
#include "gameexception.h"
#include "scriptfunctionmap.h"


class GameObjectPtr;

class GameObject : public QObject {

    Q_OBJECT

    friend class GameObjectPtr;
    friend void swap(GameObjectPtr &first, GameObjectPtr &second);

    public:
        enum Options {
            NoOptions = 0x00,
            Copy = 0x01,
            Capitalized = 0x02
        };

        explicit GameObject(const char *objectType, uint id, Options options = NoOptions);
        virtual ~GameObject();

        const char *objectType() const { return m_objectType; }
        Q_PROPERTY(const char *objectType READ objectType STORED false)

        uint id() const { return m_id; }
        Q_PROPERTY(uint id READ id STORED false)

        const QString &name() const { return m_name; }
        virtual void setName(const QString &name);
        Q_PROPERTY(QString name READ name WRITE setName)

        const QString &description() const { return m_description; }
        virtual void setDescription(const QString &description);
        Q_PROPERTY(QString description READ description WRITE setDescription)

        const ScriptFunctionMap &triggers() const { return m_triggers; }
        ScriptFunction trigger(const QString &name) const { return m_triggers[name]; }
        bool hasTrigger(const QString &name) const { return m_triggers.contains(name); }
        virtual void setTrigger(const QString &name, const ScriptFunction &function);
        virtual void unsetTrigger(const QString &name);
        virtual void setTriggers(const ScriptFunctionMap &triggers);
        Q_PROPERTY(ScriptFunctionMap triggers READ triggers WRITE setTriggers)

        Q_INVOKABLE bool invokeTrigger(const QString &triggerName,
                                       const QVariant &arg1 = QVariant(),
                                       const QVariant &arg2 = QVariant(),
                                       const QVariant &arg3 = QVariant(),
                                       const QVariant &arg4 = QVariant());
        bool invokeTrigger(const QString &triggerName,
                           GameObject *arg1, const QVariant &arg2 = QVariant(),
                           const QVariant &arg3 = QVariant(), const QVariant &arg4 = QVariant());
        bool invokeTrigger(const QString &triggerName,
                           const GameObjectPtr &arg1, const QVariant &arg2 = QVariant(),
                           const QVariant &arg3 = QVariant(), const QVariant &arg4 = QVariant());
        bool invokeTrigger(const QString &triggerName,
                           const GameObjectPtr &arg1, const GameObjectPtr &arg2,
                           const QVariant &arg3 = QVariant(), const QVariant &arg4 = QVariant());

        Q_INVOKABLE virtual void send(const QString &message, Color color = Silver);

        Q_INVOKABLE int setInterval(const QScriptValue &function, int delay);
        Q_INVOKABLE void clearInterval(int timerId);

        Q_INVOKABLE int setTimeout(const QScriptValue &function, int delay);
        Q_INVOKABLE void clearTimeout(int timerId);

        Q_INVOKABLE virtual void init();

        bool save();
        bool load(const QString &path) throw (GameException);

        void resolvePointers();

        void setDeleted();

        static GameObject *createByObjectType(const QString &objectType, uint id = 0,
                                              Options options = NoOptions) throw (GameException);

        static GameObject *createCopy(const GameObject *other);

        static GameObject *createFromFile(const QString &path) throw (GameException);

        static QScriptValue toScriptValue(QScriptEngine *engine, GameObject *const &gameObject);
        static void fromScriptValue(const QScriptValue &object, GameObject *&gameObject);

        // use with care!!
        void startBulkModification();
        void commitBulkModification();

        QList<QMetaProperty> storedMetaProperties() const;

    protected:
        virtual void timerEvent(QTimerEvent *event);

        void killAllTimers();

        void setModified();

        void setAutoDelete(bool autoDelete);

        Options options() const { return m_options; }

        void registerPointer(GameObjectPtr *pointer);
        void unregisterPointer(GameObjectPtr *pointer);

    private:
        QList<GameObjectPtr *> m_pointers;
        bool m_autoDelete;

        const char *m_objectType;
        uint m_id;
        Options m_options;

        QString m_name;
        QString m_description;
        ScriptFunctionMap m_triggers;

        int m_numBulkModifications;
        bool m_deleted;

        QHash<int, QScriptValue> *m_intervalHash;
        QHash<int, QScriptValue> *m_timeoutHash;
};

Q_DECLARE_METATYPE(GameObject *)

#endif // GAMEOBJECT_H
