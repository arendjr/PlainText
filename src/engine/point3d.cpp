#include "point3d.h"

#include <QScriptEngine>
#include <QStringList>

#include "gameexception.h"


Point3D::Point3D(int x, int y, int z) :
    x(x),
    y(y),
    z(z) {
}

bool Point3D::operator==(const Point3D &other) const {

    return other.x == x && other.y == y && other.z == z;
}

bool Point3D::operator!=(const Point3D &other) const {

    return other.x != x || other.y != y || other.z != z;
}

Point3D Point3D::operator+(const Vector3D &vector) const {

    return Point3D(x + vector.x, y + vector.y, z + vector.z);
}

Point3D Point3D::operator-(const Vector3D &vector) const {

    return Point3D(x - vector.x, y - vector.y, z - vector.z);
}

Vector3D Point3D::operator-(const Point3D &other) const {

    return Vector3D(x - other.x, y - other.y, z - other.z);
}

QString Point3D::toString() const {

    return QString("( %1, %2, %3 )").arg(x).arg(y).arg(z);
}

QString Point3D::toUserString(const Point3D &point) {

    return point.toString();
}

Point3D Point3D::fromUserString(const QString &string) {

    if (!string.startsWith("(") || !string.endsWith(")")) {
        throw GameException(GameException::InvalidPoint);
    }

    QStringList stringList = string.mid(1, string.length() - 2).split(',');
    if (stringList.length() != 3) {
        throw GameException(GameException::InvalidPoint);
    }

    return Point3D(stringList[0].trimmed().toInt(),
                   stringList[1].trimmed().toInt(),
                   stringList[2].trimmed().toInt());
}

QString Point3D::toJsonString(const Point3D &point, Options options) {

    Q_UNUSED(options)

    return QString("[ %1, %2, %3 ]").arg(point.x).arg(point.y).arg(point.z);
}

Point3D Point3D::fromVariant(const QVariant &variant) {

    QVariantList variantList = variant.toList();
    if (variantList.length() != 3) {
        throw GameException(GameException::InvalidPoint);
    }

    return Point3D(variantList[0].toInt(), variantList[1].toInt(), variantList[2].toInt());
}

QScriptValue Point3D::toScriptValue(QScriptEngine *engine, const Point3D &point) {

    QScriptValue object = engine->newArray(3);
    object.setProperty(0, point.x);
    object.setProperty(1, point.y);
    object.setProperty(2, point.z);
    return object;
}

void Point3D::fromScriptValue(const QScriptValue &object, Point3D &point) {

    point.x = object.property(0).toInt32();
    point.y = object.property(1).toInt32();
    point.z = object.property(2).toInt32();
}
