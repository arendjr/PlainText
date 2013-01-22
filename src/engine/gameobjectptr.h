#ifndef GAMEOBJECTPTR_H
#define GAMEOBJECTPTR_H

#include <QList>
#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
#include <QString>
#include <QVariant>

#include "constants.h"
#include "gameexception.h"
#include "gameobject.h"
#include "metatyperegistry.h"


class GameObject;
class GameObjectPtrList;
class Realm;

class GameObjectPtr {

    public:
        GameObjectPtr();
        GameObjectPtr(GameObject *gameObject);
        GameObjectPtr(Realm *realm, GameObjectType objectType, uint id);
        GameObjectPtr(const GameObjectPtr &other);
        GameObjectPtr(GameObjectPtr &&other);
        ~GameObjectPtr();

        bool isNull() const;

        GameObjectPtr &operator=(const GameObjectPtr &other);
        GameObjectPtr &operator=(GameObjectPtr &&other);

        bool operator==(const GameObjectPtr &other) const;
        bool operator==(const GameObject *other) const;
        bool operator!=(const GameObjectPtr &other) const;
        bool operator!=(const GameObject *other) const;

        inline GameObject &operator*() const {
            if (!m_gameObject) {
                throw GameException(GameException::NullPointerReference);
            }
            return *m_gameObject;
        }

        inline GameObject *operator->() const {
            if (!m_gameObject) {
                throw GameException(GameException::NullPointerReference);
            }
            return m_gameObject;
        }

        template <class T> inline T cast() const {
            if (!m_gameObject) {
                throw GameException(GameException::NullPointerReference);
            }
            T pointer = qobject_cast<T>(m_gameObject);
            if (!pointer) {
                throw GameException(GameException::InvalidGameObjectCast, m_objectType, m_id);
            }
            return pointer;
        }

        template <class T> inline T unsafeCast() const {
            return static_cast<T>(m_gameObject);
        }

        void resolve(Realm *realm);
        void unresolve(bool unregister = true);

        void setOwnerList(GameObjectPtrList *list);

        QString toString() const;

        static QString toUserString(const GameObjectPtr &pointer);
        static void fromUserString(const QString &string, GameObjectPtr &pointer);

        static QString toJsonString(const GameObjectPtr &pointer, Options options = NoOptions);
        static void fromVariant(const QVariant &variant, GameObjectPtr &pointer);

        static QScriptValue toScriptValue(QScriptEngine *engine, const GameObjectPtr &pointer);
        static void fromScriptValue(const QScriptValue &object, GameObjectPtr &pointer);

        friend void swap(GameObjectPtr &first, GameObjectPtr &second);
        friend void swapWithinList(GameObjectPtr &first, GameObjectPtr &second);

    private:
        GameObject *m_gameObject;

        GameObjectType m_objectType;
        uint m_id;

        GameObjectPtrList *m_list;
};

PT_DECLARE_SERIALIZABLE_METATYPE(GameObjectPtr)


class GameObjectPtrList {

    public:
        class const_iterator;

        class iterator {

            public:
                friend class GameObjectPtrList;

                bool operator!=(const iterator &other) const;
                bool operator!=(const const_iterator &other) const;

                GameObjectPtr &operator*() const;

                iterator &operator++();
                iterator operator++(int);

                bool operator==(const iterator &other) const;
                bool operator==(const const_iterator &other) const;

            private:
                GameObjectPtrList *m_list;
                int m_index;
        };

        typedef iterator Iterator;


        class const_iterator {

            public:
                friend class GameObjectPtrList;

                const_iterator() = default;
                const_iterator(const iterator &other);

                bool operator!=(const const_iterator &other) const;

                const GameObjectPtr &operator*() const;

                const_iterator &operator++();
                const_iterator operator++(int);

                bool operator==(const const_iterator &other) const;

            private:
                GameObjectPtrList *m_list;
                int m_index;
        };

        typedef const_iterator ConstIterator;


        typedef GameObjectPtr value_type;


        friend class iterator;
        friend class const_iterator;


        GameObjectPtrList();
        explicit GameObjectPtrList(int size);
        GameObjectPtrList(const GameObjectPtrList &other);
        GameObjectPtrList(GameObjectPtrList &&other);
        ~GameObjectPtrList();

        void append(const GameObjectPtr &value);
        void append(const GameObjectPtrList &value);

        iterator begin();
        const_iterator begin() const;

        void clear();

        const_iterator constBegin() const;
        const_iterator constEnd() const;

        bool contains(const GameObjectPtr &value) const;

        iterator end();
        const_iterator end() const;

        GameObjectPtr &first();
        const GameObjectPtr &first() const;

        int indexOf(const GameObjectPtr &value) const;

        void insert(const GameObjectPtr &value);

        bool isEmpty() const;

        GameObjectPtr &last();
        const GameObjectPtr &last() const;

        int length() const;

        inline void push_back(const GameObjectPtr &value) {
            append(value);
        }

        int removeAll(const GameObjectPtr &value);
        void removeAt(int i);
        bool removeOne(const GameObjectPtr &value);

        void reserve(int size);

        int size() const;

        friend void swap(GameObjectPtrList &first, GameObjectPtrList &second);

        bool operator!=(const GameObjectPtrList &other) const;

        GameObjectPtrList operator+(const GameObjectPtrList &other) const;

        inline GameObjectPtrList &operator<<(const GameObjectPtrList &other) {
            append(other);
            return *this;
        }
        inline GameObjectPtrList &operator<<(const GameObjectPtr &value) {
            append(value);
            return *this;
        }

        GameObjectPtrList &operator=(const GameObjectPtrList &other);
        GameObjectPtrList &operator=(GameObjectPtrList &&other);

        bool operator==(const GameObjectPtrList &other) const;

        const GameObjectPtr &operator[](int i) const;

        void resolvePointers(Realm *realm);
        void unresolvePointers();

        void send(const QString &message, int color = Silver) const;

        QString joinFancy(Options options = NoOptions) const;

        static QString toUserString(const GameObjectPtrList &pointerList);
        static void fromUserString(const QString &string, GameObjectPtrList &pointerList);

        static QString toJsonString(const GameObjectPtrList &pointerList,
                                    Options options = NoOptions);
        static void fromVariant(const QVariant &variant, GameObjectPtrList &pointerList);

    private:
        int m_size;
        int m_capacity;
        GameObjectPtr *m_items;
        GameObjectPtrList *m_nextList;
};

PT_DECLARE_SERIALIZABLE_METATYPE(GameObjectPtrList)

#endif // GAMEOBJECTPTR_H
