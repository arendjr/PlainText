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

        static QString fromIndex(const QStringList &list, int index,
                                 const QString &separator = " ");

        static QString jsString(QString string);

        static QString colorize(const QString &string, Color color);

    private:
        Util();
};

#endif // UTIL_H
