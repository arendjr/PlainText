#ifndef GAMEOBJECTPTR_H
#define GAMEOBJECTPTR_H

#include <QList>
#include <QMetaType>
#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
#include <QString>

#include "gameexception.h"


class GameObject;

class GameObjectPtr {

    public:
        GameObjectPtr();
        GameObjectPtr(GameObject *gameObject);
        GameObjectPtr(const char *objectType, uint id);
        GameObjectPtr(const GameObjectPtr &other);
        ~GameObjectPtr();

        bool isNull() const;

        GameObjectPtr &operator=(const GameObjectPtr &other);
        bool operator==(const GameObjectPtr &other) const;
        bool operator==(const GameObject *other) const;
        bool operator!=(const GameObjectPtr &other) const;
        bool operator!=(const GameObject *other) const;

        inline GameObject &operator*() const throw (GameException) {
            if (!m_gameObject) {
                throw GameException(GameException::NullPointerReference);
            }
            return *m_gameObject;
        }

        inline GameObject *operator->() const throw (GameException) {
            if (!m_gameObject) {
                throw GameException(GameException::NullPointerReference);
            }
            return m_gameObject;
        }

        template <class T> inline T cast() const throw (GameException) {
            if (!m_gameObject) {
                throw GameException(GameException::NullPointerReference);
            }
            T pointer = qobject_cast<T>(m_gameObject);
            if (!pointer) {
                throw GameException(GameException::InvalidGameObjectCast, m_objectType, m_id);
            }
            return pointer;
        }

        void resolve() throw (GameException);

        QString toString() const;
        static GameObjectPtr fromString(const QString &string) throw (GameException);

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
