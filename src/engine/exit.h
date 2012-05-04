#ifndef EXIT_H
#define EXIT_H

#include <QMetaType>
#include <QString>
#include <QVariantList>

#include "gameobjectptr.h"


class Exit {

    public:
        Exit();

        bool operator==(const Exit &other) const;

        const QString &name() const { return m_name; };
        void setName(const QString &name);

        const GameObjectPtr &destinationArea() const { return m_destinationArea; };
        void setDestinationArea(const GameObjectPtr &destinationArea);

        bool isHidden() const { return m_hidden; };
        void setHidden(bool hidden);

        void resolvePointer();

        QString toString() const;

        QVariantList toVariantList() const;

        static Exit fromVariantList(const QVariantList &list);

    private:
        QString m_name;
        GameObjectPtr m_destinationArea;
        bool m_hidden;
};

typedef QList<Exit> ExitList;

Q_DECLARE_METATYPE(Exit);
Q_DECLARE_METATYPE(ExitList);

#endif // EXIT_H
