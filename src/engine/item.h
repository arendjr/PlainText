#ifndef ITEM_H
#define ITEM_H

#include "gameobject.h"
#include "gameobjectptr.h"


class Item : public GameObject {

    Q_OBJECT

    public:
        Item(Realm *realm, uint id, Options options = NoOptions);
        Item(Realm *realm, const char *objectType, uint id, Options options = NoOptions);
        virtual ~Item();

        Q_INVOKABLE QString definiteName(const GameObjectPtrList &pool,
                                         Options options = NoOptions);
        Q_INVOKABLE QString indefiniteName(Options options = NoOptions);

        const QString &plural() const { return m_plural; }
        void setPlural(const QString &plural);
        Q_PROPERTY(QString plural READ plural WRITE setPlural)

        const QString &indefiniteArticle() const { return m_indefiniteArticle; }
        void setIndefiniteArticle(const QString &indefiniteArticle);
        Q_PROPERTY(QString indefiniteArticle READ indefiniteArticle WRITE setIndefiniteArticle)

        bool isPortable() const { return m_portable; }
        void setPortable(bool portable);
        Q_PROPERTY(bool portable READ isPortable WRITE setPortable)

        int weight() const { return m_weight; }
        void adjustWeight(int delta);
        void setWeight(int weight);
        Q_PROPERTY(int weight READ weight WRITE setWeight)

        double cost() const { return m_cost; }
        void setCost(double cost);
        Q_PROPERTY(double cost READ cost WRITE setCost)

        static Item *createGold(Realm *realm, double amount);

    protected:
        virtual void changeName(const QString &newName);

    private:
        QString m_plural;
        QString m_indefiniteArticle;

        bool m_portable;

        int m_weight;

        double m_cost;
};

#endif // ITEM_H
