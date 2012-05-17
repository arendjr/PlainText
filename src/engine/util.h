#ifndef UTIL_H
#define UTIL_H

#include <QStringList>

#include "gameobjectptr.h"


enum Color {
    White = 0,
    Silver,
    Gray,
    Black,
    Red,
    Maroon,
    Yellow,
    Olive,
    Lime,
    Green,
    Aqua,
    Teal,
    Blue,
    Navy,
    Fuchsia,
    Purple
};

enum Articles {
    IndefiniteArticle,
    DefiniteArticle
};

class Util {

    public:
        static QString joinFancy(const QStringList &list,
                                 const QString &separator = ", ",
                                 const QString &last = " and ");

        static QString joinItems(const GameObjectPtrList &items, Articles article = IndefiniteArticle);

        static QString writtenNumber(int number);

        static QString capitalize(const QString &string);

        static QString jsString(QString string);

        static QString colorize(const QString &string, Color color);

        static bool isDirection(const QString &string);

        static QString opposingDirection(const QString &direction);

        static bool isDirectionAbbreviation(const QString &string);

        static QString direction(const QString &abbreviation);

        static QString toCamelCase(QString string);

        static void sendOthers(GameObjectPtrList characters, const QString &string,
                               const GameObjectPtr &exclude1 = GameObjectPtr(),
                               const GameObjectPtr &exclude2 = GameObjectPtr());

    private:
        Util();
};

#endif // UTIL_H
