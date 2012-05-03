#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QObject>


class GameObject : public QObject {

    Q_OBJECT

    public:
        explicit GameObject(const char *objectType, uint id, QObject *parent = 0);
        virtual ~GameObject();

        const char *objectType() const { return m_objectType; }
        const uint id() const { return m_id; }

        bool save();
        bool load(const QString &path);

        void resolvePointers();

        static GameObject *createByObjectType(const QString &objectType, uint id = 0);

        static GameObject *createFromFile(const QString &path);

    protected:
        void setModified();

    private:
        bool m_saved; // object saved to disk?

        const char *m_objectType;
        uint m_id;
};

#endif // GAMEOBJECT_H
