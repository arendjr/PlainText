#ifndef ITEM_H
#define ITEM_H

#include "gameobject.h"


class Item : public GameObject {

    Q_OBJECT

    public:
        explicit Item(uint id, Options options = NoOptions);
        virtual ~Item();

        virtual void setName(const QString &newName);

        const QString &plural() const { return m_plural; }
        virtual void setPlural(const QString &plural);
        Q_PROPERTY(QString plural READ plural WRITE setPlural)

        const QString &indefiniteArticle() const { return m_indefiniteArticle; }
        virtual void setIndefiniteArticle(const QString &indefiniteArticle);
        Q_PROPERTY(QString indefiniteArticle READ indefiniteArticle WRITE setIndefiniteArticle)

        bool isPortable() const { return m_portable; }
        virtual void setPortable(bool portable);
        Q_PROPERTY(bool portable READ isPortable WRITE setPortable)

        int weight() const { return m_weight; }
        virtual void adjustWeight(int delta);
        virtual void setWeight(int weight);
        Q_PROPERTY(int weight READ weight WRITE setWeight)

    protected:
        explicit Item(const char *objectType, uint id, Options options = NoOptions);

    private:
        QString m_plural;
        QString m_indefiniteArticle;

        bool m_portable;

        int m_weight;
};

#endif // ITEM_H
