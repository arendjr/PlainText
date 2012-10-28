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

    QStringList components;
    components << QString::number(x);
    components << QString::number(y);
    components << QString::number(z);
    return "[" + components.join(", ") + "]";
}

Point3D Point3D::fromString(const QString &string) {

    if (!string.startsWith("[") || !string.endsWith("]")) {
        throw GameException(GameException::InvalidPoint);
    }

    QStringList components = string.mid(1, -1).split(',');
    if (components.length() != 3) {
        throw GameException(GameException::InvalidPoint);
    }

    Point3D point;
    point.x = components[0].trimmed().toInt();
    point.y = components[1].trimmed().toInt();
    point.z = components[2].trimmed().toInt();
    return point;
}

Point3D Point3D::fromVariantList(const QVariantList &variantList) {

    if (variantList.length() != 3) {
        throw GameException(GameException::InvalidPoint);
    }

    Point3D point;
    point.x = variantList[0].toInt();
    point.y = variantList[1].toInt();
    point.z = variantList[2].toInt();
    return point;
}

QScriptValue Point3D::toScriptValue(QScriptEngine *engine, const Point3D &point) {

    QScriptValue object = engine->newObject();
    object.setProperty("x", point.x);
    object.setProperty("y", point.y);
    object.setProperty("z", point.z);
    return object;
}

void Point3D::fromScriptValue(const QScriptValue &object, Point3D &point) {

    point.x = object.property("x").toInt32();
    point.y = object.property("y").toInt32();
    point.z = object.property("z").toInt32();
}
