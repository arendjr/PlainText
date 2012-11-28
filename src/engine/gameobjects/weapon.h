#ifndef WEAPON_H
#define WEAPON_H

#include "statsitem.h"


class Weapon : public StatsItem {

    Q_OBJECT

    public:
        Weapon(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Weapon();

        const QString &category() const { return m_category; }
        void setCategory(const QString &category);
        Q_PROPERTY(QString category READ category WRITE setCategory)

    private:
        QString m_category;
};

#endif // WEAPON_H
