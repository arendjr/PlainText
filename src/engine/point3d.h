#ifndef POINT3D_H
#define POINT3D_H

#include <QMetaType>
#include <QScriptValue>
#include <QString>
#include <QVariantList>

#include "vector3d.h"


class Point3D {

    public:
        int x;
        int y;
        int z;

        Point3D() = default;
        Point3D(int x, int y, int z);

        bool operator==(const Point3D &other) const;
        bool operator!=(const Point3D &other) const;

        Point3D operator+(const Vector3D &vector) const;
        Point3D operator-(const Vector3D &vector) const;

        Vector3D operator-(const Point3D &other) const;

        QString toString() const;
        static Point3D fromString(const QString &string);

        static Point3D fromVariantList(const QVariantList &variantList);

        static QScriptValue toScriptValue(QScriptEngine *engine, const Point3D &stats);
        static void fromScriptValue(const QScriptValue &object, Point3D &stats);
};

Q_DECLARE_METATYPE(Point3D)

#endif // POINT3D_H
