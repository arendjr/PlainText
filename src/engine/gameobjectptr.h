#ifndef GAMEOBJECTPTR_H
#define GAMEOBJECTPTR_H

#include <QList>
#include <QMetaType>
#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
#include <QString>

#include "constants.h"
#include "gameexception.h"


class GameObject;
class GameObjectPtrList;

class GameObjectPtr {

    public:
        friend class GameObjectPtrList;

        GameObjectPtr();
        GameObjectPtr(GameObject *gameObject);
        GameObjectPtr(const char *objectType, uint id);
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
        void unresolve(bool unregister = true);

        QString toString() const;
        static GameObjectPtr fromString(const QString &string) throw (GameException);

        friend void swap(GameObjectPtr &first, GameObjectPtr &second);

        static QScriptValue toScriptValue(QScriptEngine *engine, const GameObjectPtr &pointer);
        static void fromScriptValue(const QScriptValue &object, GameObjectPtr &pointer);

    private:
        GameObject *m_gameObject;

        const char *m_objectType;
        uint m_id;

        GameObjectPtrList *m_list;
};

Q_DECLARE_METATYPE(GameObjectPtr)


class GameObjectPtrList {

    public:
        class const_iterator;

        class iterator {

            public:
                friend class GameObjectPtrList;

                iterator();
                iterator(const iterator &other);

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

                const_iterator();
                const_iterator(const const_iterator &other);
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

        int indexOf(const GameObjectPtr &value, int from = 0) const;

        bool isEmpty() const;

        int length() const;

        void push_back(const GameObjectPtr &value);

        int removeAll(const GameObjectPtr &value);
        void removeAt(int i);
        bool removeOne(const GameObjectPtr &value);

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

        void resolvePointers() throw (GameException);
        void unresolvePointers();

        void send(const QString &message, Color color = Silver);

    private:
        static const int NUM_ITEMS = 16;

        int m_size;
        GameObjectPtr m_items[NUM_ITEMS];
        GameObjectPtrList *m_nextList;
};

Q_DECLARE_METATYPE(GameObjectPtrList)


//
// works just like Qt's foreach keyword ...
//
// ... except:
// - the container is not copied (this is faster, but modifying the list is
//   unsupported)
// - the variable type is always const GameObjectPtr &
//
class GOPLForeachContainer {

    public:
        inline GOPLForeachContainer(const GameObjectPtrList &t) :
            c(t),
            brk(0),
            i(c.begin()),
            e(c.end()) {
        }

        const GameObjectPtrList &c;
        int brk;
        typename GameObjectPtrList::const_iterator i, e;
};

#define gopl_foreach(variable, container) \
for (GOPLForeachContainer _container_(container); \
     !_container_.brk && _container_.i != _container_.e; \
     __extension__  ({ ++_container_.brk; ++_container_.i; })) \
    for (const GameObjectPtr &variable = *_container_.i;; \
         __extension__ ({ --_container_.brk; break; }))

#endif // GAMEOBJECTPTR_H
