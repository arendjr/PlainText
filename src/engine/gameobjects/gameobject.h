#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QHash>
#include <QMetaProperty>
#include <QObject>
#include <QScriptEngine>
#include <QVariantMap>
#include <QVector>

#include "constants.h"
#include "metatyperegistry.h"
#include "scriptfunctionmap.h"


class GameObjectPtr;
class GameObjectPtrList;
class Realm;


PT_DEFINE_ENUM(GameObjectType,
    Unknown,
    Portal,
    Exit,
    Room,
    Item,
    Event,
    Character,
    Player,
    Class,
    Race,
    Container,
    Group,
    Weapon,
    Shield,
    Realm
)


class GameObject : public QObject {

    Q_OBJECT

    friend class GameObjectPtr;
    friend void swap(GameObjectPtr &first, GameObjectPtr &second);
    friend void swapWithinList(GameObjectPtr &first, GameObjectPtr &second);

    public:
        GameObject(Realm *realm, GameObjectType objectType, uint id, Options options = NoOptions);
        virtual ~GameObject();

        Realm *realm() const { return m_realm; }

        GameObjectType objectType() const { return m_objectType; }
        Q_PROPERTY(GameObjectType objectType READ objectType STORED false)

        Q_INVOKABLE bool isClass() const;
        Q_INVOKABLE bool isContainer() const;
        Q_INVOKABLE bool isEvent() const;
        Q_INVOKABLE bool isExit() const;
        Q_INVOKABLE bool isItem() const;
        Q_INVOKABLE bool isCharacter() const;
        Q_INVOKABLE bool isGroup() const;
        Q_INVOKABLE bool isPlayer() const;
        Q_INVOKABLE bool isPortal() const;
        Q_INVOKABLE bool isRace() const;
        Q_INVOKABLE bool isRealm() const;
        Q_INVOKABLE bool isRoom() const;
        Q_INVOKABLE bool isShield() const;
        Q_INVOKABLE bool isWeapon() const;
        Q_INVOKABLE bool hasStats() const;

        uint id() const { return m_id; }
        Q_PROPERTY(uint id READ id STORED false)

        const QString &name() const { return m_name; }
        void setName(const QString &name);
        Q_PROPERTY(QString name READ name WRITE setName)

        Q_INVOKABLE QString definiteName(const GameObjectPtrList &pool,
                                         int options = NoOptions) const;
        Q_INVOKABLE QString indefiniteName(int options = NoOptions) const;

        const QString &plural() const { return m_plural; }
        void setPlural(const QString &plural);
        Q_PROPERTY(QString plural READ plural WRITE setPlural)

        const QString &indefiniteArticle() const { return m_indefiniteArticle; }
        void setIndefiniteArticle(const QString &indefiniteArticle);
        Q_PROPERTY(QString indefiniteArticle READ indefiniteArticle WRITE setIndefiniteArticle)

        const QString &description() const { return m_description; }
        void setDescription(const QString &description);
        Q_PROPERTY(QString description READ description WRITE setDescription)

        const QVariantMap &data() const { return m_data; }
        void setData(const QVariantMap &data);
        Q_INVOKABLE void setBoolData(const QString &name, bool value);
        Q_INVOKABLE void setIntData(const QString &name, int value);
        Q_INVOKABLE void setStringData(const QString &name, const QString &value);
        Q_INVOKABLE void setGameObjectData(const QString &name, const GameObjectPtr &value);
        Q_INVOKABLE void setGameObjectListData(const QString &name, const GameObjectPtrList &value);
        Q_PROPERTY(QVariantMap data READ data WRITE setData)

        const ScriptFunctionMap &triggers() const { return m_triggers; }
        ScriptFunction trigger(const QString &name) const { return m_triggers[name]; }
        Q_INVOKABLE bool hasTrigger(const QString &name) const { return m_triggers.contains(name); }
        Q_INVOKABLE void setTrigger(const QString &name, const ScriptFunction &function);
        Q_INVOKABLE void unsetTrigger(const QString &name);
        void setTriggers(const ScriptFunctionMap &triggers);
        Q_PROPERTY(ScriptFunctionMap triggers READ triggers WRITE setTriggers)

        Q_INVOKABLE bool invokeTrigger(const QString &triggerName,
                                       const QScriptValue &arg1 = QScriptValue(),
                                       const QScriptValue &arg2 = QScriptValue(),
                                       const QScriptValue &arg3 = QScriptValue(),
                                       const QScriptValue &arg4 = QScriptValue());
        bool invokeTrigger(const QString &triggerName,
                           GameObject *arg1,
                           const GameObjectPtr &arg2,
                           const QScriptValue &arg3 = QScriptValue(),
                           const QScriptValue &arg4 = QScriptValue());
        bool invokeTrigger(const QString &triggerName,
                           GameObject *arg1,
                           const GameObjectPtrList &arg2,
                           const QScriptValue &arg3 = QScriptValue(),
                           const QScriptValue &arg4 = QScriptValue());
        bool invokeTrigger(const QString &triggerName,
                           GameObject *arg1,
                           const GameObjectPtr &arg2,
                           const GameObjectPtrList &arg3,
                           const QScriptValue &arg4 = QScriptValue());
        bool invokeTrigger(const QString &triggerName,
                           GameObject *arg1,
                           const QScriptValue &arg2 = QScriptValue(),
                           const QScriptValue &arg3 = QScriptValue(),
                           const QScriptValue &arg4 = QScriptValue());
        bool invokeTrigger(const QString &triggerName,
                           const GameObjectPtr &arg1,
                           const GameObjectPtr &arg2,
                           const QScriptValue &arg3 = QScriptValue(),
                           const QScriptValue &arg4 = QScriptValue());
        bool invokeTrigger(const QString &triggerName,
                           const GameObjectPtr &arg1,
                           const QScriptValue &arg2 = QScriptValue(),
                           const QScriptValue &arg3 = QScriptValue(),
                           const QScriptValue &arg4 = QScriptValue());

        bool hasScriptMethod(const QString &methodName);
        QScriptValue invokeScriptMethod(const QString &methodName,
                                        const QScriptValue &arg1 = QScriptValue(),
                                        const QScriptValue &arg2 = QScriptValue(),
                                        const QScriptValue &arg3 = QScriptValue(),
                                        const QScriptValue &arg4 = QScriptValue());

        Q_INVOKABLE virtual void send(const QString &message, int color = Silver) const;

        Q_INVOKABLE virtual QString lookAtBy(GameObject *character);

        Q_INVOKABLE int setInterval(const QScriptValue &function, int delay);
        Q_INVOKABLE void clearInterval(int intervalId);

        Q_INVOKABLE int setTimeout(const QScriptValue &function, int delay);
        Q_INVOKABLE void clearTimeout(int timerId);

        Q_INVOKABLE virtual void init();

        Q_INVOKABLE virtual GameObject *copy();

        QString toJsonString(Options options = NoOptions) const;

        bool save();
        bool load(const QString &path);

        void resolvePointers();

        Q_INVOKABLE void setDeleted();

        QVector<QMetaProperty> metaProperties() const;
        QVector<QMetaProperty> storedMetaProperties() const;

        virtual void invokeTimer(int timerId);

        virtual void killAllTimers();

        static GameObject *createByObjectType(Realm *realm, GameObjectType objectType, uint id = 0,
                                              Options options = NoOptions);

        static GameObject *createFromFile(Realm *realm, const QString &path);

        static GameObject *createCopy(const GameObject *other);

        static QScriptValue toScriptValue(QScriptEngine *engine, GameObject *const &gameObject);
        static void fromScriptValue(const QScriptValue &object, GameObject *&gameObject);

        static void clearPrototypeMap();

    protected:
        bool mayReferenceOtherProperties() const;
        void setModified();

        void setAutoDelete(bool autoDelete);

        Options options() const { return m_options; }

        void registerPointer(GameObjectPtr *pointer);
        void unregisterPointer(GameObjectPtr *pointer);

        virtual void changeName(const QString &newName);

    private:
        Realm *m_realm;

        GameObjectType m_objectType;
        uint m_id;
        Options m_options;

        bool m_deleted;

        QVector<GameObjectPtr *> m_pointers;

        QString m_name;
        QString m_plural;
        QString m_indefiniteArticle;
        QString m_description;
        QVariantMap m_data;
        ScriptFunctionMap m_triggers;

        QHash<int, QScriptValue> *m_intervalHash;
        QHash<int, QScriptValue> *m_timeoutHash;

        static QMap<QString, QScriptValue> s_prototypeMap;
};

Q_DECLARE_METATYPE(GameObject *)

#endif // GAMEOBJECT_H
