#ifndef GAMEOBJECTPTR_H
#define GAMEOBJECTPTR_H

#include <QList>
#include <QMetaType>
#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
#include <QString>

#include "badgameobjectexception.h"


class GameObject;

class GameObjectPtr {

    public:
        GameObjectPtr();
        GameObjectPtr(GameObject *gameObject);
        GameObjectPtr(const char *objectType, uint id);
        GameObjectPtr(const GameObjectPtr &other);
        ~GameObjectPtr();

        GameObjectPtr &operator=(GameObjectPtr other);
        bool operator==(const GameObjectPtr &other) const;
        bool operator==(const GameObject *other) const;
        bool operator!=(const GameObjectPtr &other) const;
        bool operator!=(const GameObject *other) const;

        GameObject &operator*() const { Q_ASSERT(m_gameObject); return *m_gameObject; }
        GameObject *operator->() const { Q_ASSERT(m_gameObject); return m_gameObject; }

        template <class T> T cast() const { Q_ASSERT(m_gameObject); return qobject_cast<T>(m_gameObject); }

        void resolve() throw (BadGameObjectException);

        QString toString() const;

        static GameObjectPtr fromString(const QString &string) throw (BadGameObjectException);

        friend void swap(GameObjectPtr &first, GameObjectPtr &second);

        static QScriptValue toScriptValue(QScriptEngine *engine, const GameObjectPtr &pointer);
        static void fromScriptValue(const QScriptValue &object, GameObjectPtr &pointer);

    private:
        GameObject *m_gameObject;

        const char *m_objectType;
        uint m_id;
};

typedef QList<GameObjectPtr> GameObjectPtrList;

Q_DECLARE_METATYPE(GameObjectPtr)
Q_DECLARE_METATYPE(GameObjectPtrList)

#endif // GAMEOBJECTPTR_H
