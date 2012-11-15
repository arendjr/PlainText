#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QHash>
#include <QMetaProperty>
#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
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
    Exit,
    Portal,
    Room,
    Item,
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

        Q_INVOKABLE bool isExit() const;
        Q_INVOKABLE bool isRealm() const;
        Q_INVOKABLE bool isRoom() const;
        Q_INVOKABLE bool isPortal() const;

        uint id() const { return m_id; }
        Q_PROPERTY(uint id READ id STORED false)

        const QString &name() const { return m_name; }
        void setName(const QString &name);
        Q_PROPERTY(QString name READ name WRITE setName)

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
        void setTrigger(const QString &name, const ScriptFunction &function);
        void unsetTrigger(const QString &name);
        void setTriggers(const ScriptFunctionMap &triggers);
        Q_PROPERTY(ScriptFunctionMap triggers READ triggers WRITE setTriggers)

        QString toJsonString(Options options = NoOptions) const;

        bool save();
        bool load(const QString &path);

        void resolvePointers();

        Q_INVOKABLE void setDeleted();

        static GameObject *createByObjectType(Realm *realm, GameObjectType objectType, uint id = 0,
                                              Options options = NoOptions);

        static GameObject *createFromFile(Realm *realm, const QString &path);

        static QScriptValue toScriptValue(QScriptEngine *engine, GameObject *const &gameObject);
        static void fromScriptValue(const QScriptValue &object, GameObject *&gameObject);

        QList<QMetaProperty> metaProperties() const;
        QList<QMetaProperty> storedMetaProperties() const;

    protected:
        bool mayReferenceOtherProperties() const;
        void setModified();

        void setAutoDelete(bool autoDelete);

        Options options() const { return m_options; }

        void registerPointer(GameObjectPtr *pointer);
        void unregisterPointer(GameObjectPtr *pointer);

    private:
        Realm *m_realm;

        GameObjectType m_objectType;
        uint m_id;
        Options m_options;

        bool m_autoDelete;
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
};

Q_DECLARE_METATYPE(GameObject *)

#endif // GAMEOBJECT_H
