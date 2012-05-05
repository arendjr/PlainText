#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QList>
#include <QMetaProperty>
#include <QObject>


class GameObject : public QObject {

    Q_OBJECT

    public:
        enum Options {
            NoOptions = 0x00,
            Copy = 0x01
        };

        explicit GameObject(const char *objectType, uint id, Options options = NoOptions);
        virtual ~GameObject();

        const char *objectType() const { return m_objectType; }
        const uint id() const { return m_id; }

        bool save();
        bool load(const QString &path);

        void resolvePointers();

        static GameObject *createByObjectType(const QString &objectType, uint id = 0,
                                              Options options = NoOptions);

        static GameObject *createCopy(const GameObject *other);

        static GameObject *createFromFile(const QString &path);

    protected:
        void setModified();

        Options options() const { return m_options; }

    private:
        const char *m_objectType;
        uint m_id;
        Options m_options;

        QList<QMetaProperty> storedMetaProperties() const;
};

#endif // GAMEOBJECT_H
