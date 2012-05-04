#ifndef REALM_H
#define REALM_H

#include <QMap>

#include "gameobject.h"


class Character;

class Realm : public GameObject {

    Q_OBJECT

    public:
        static Realm *instance() { Q_ASSERT(s_instance); return s_instance; };

        static void instantiate();
        static void destroy();

        bool isInitialized() const { return m_initialized; }

        void registerObject(GameObject *gameObject);
        void unregisterObject(GameObject *gameObject);
        GameObject *getObject(const char *objectType, uint id) const;

        void registerCharacter(Character *character);
        void unregisterCharacter(Character *character);
        Character *getCharacter(const QString &name) const;

        uint uniqueObjectId();

        static QString saveDirPath();
        static QString saveObjectPath(const char *objectType, uint id);

    private:
        static Realm *s_instance;

        bool m_initialized;

        uint m_nextId;
        QMap<uint, GameObject *> m_objectMap;
        QMap<QString, Character *> m_characterMap;

        explicit Realm();
        virtual ~Realm();
};

#endif // REALM_H
