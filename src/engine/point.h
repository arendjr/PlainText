#ifndef POINT_H
#define POINT_H

#include <QMetaType>
#include <QScriptValue>
#include <QString>
#include <QVariantList>


class Point {

    public:
        int x;
        int y;
        int z;

        Point();
        Point(int x, int y, int z);
        Point(const Point &other);

        Point &operator=(const Point &other);
        bool operator==(const Point &other) const;
        bool operator!=(const Point &other) const;

        Point operator-(const Point &other) const;

        QString toString() const;
        static Point fromString(const QString &string);

        static Point fromVariantList(const QVariantList &variantList);

        static QScriptValue toScriptValue(QScriptEngine *engine, const Point &stats);
        static void fromScriptValue(const QScriptValue &object, Point &stats);
};

Q_DECLARE_METATYPE(Point)

#endif // POINT_H
