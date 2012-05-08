#ifndef UTIL_H
#define UTIL_H

#include <QStringList>


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

class Util {

    public:
        static QString joinFancy(const QStringList &list,
                                 const QString &separator = ", ",
                                 const QString &last = " and ");

        static QString jsString(QString string);

        static QString colorize(const QString &string, Color color);

        static bool isDirection(const QString &string);

        static QString opposingDirection(const QString &direction);

        static bool isDirectionAbbreviation(const QString &string);

        static QString direction(const QString &abbreviation);

    private:
        Util();
};

#endif // UTIL_H
