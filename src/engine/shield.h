#ifndef SHIELD_H
#define SHIELD_H

#include "statsitem.h"


class Shield : public StatsItem {

    Q_OBJECT

    public:
        Shield(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Shield();

        const QString &category() const { return m_category; }
        void setCategory(const QString &category);
        Q_PROPERTY(QString category READ category WRITE setCategory)

    private:
        QString m_category;
};

#endif // SHIELD_H
