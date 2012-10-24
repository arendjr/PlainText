#include "point.h"

#include <QScriptEngine>
#include <QStringList>

#include "gameexception.h"


Point::Point() :
    x(0),
    y(0),
    z(0) {
}

Point::Point(int x, int y, int z) :
    x(x),
    y(y),
    z(z) {
}

Point::Point(const Point &other) :
    x(other.x),
    y(other.y),
    z(other.z) {
}

Point &Point::operator=(const Point &other) {

    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

bool Point::operator==(const Point &other) const {

    return other.x == x && other.y == y && other.z == z;
}

bool Point::operator!=(const Point &other) const {

    return other.x != x || other.y != y || other.z != z;
}

Point Point::operator-(const Point &other) const {

    Point point;
    point.x = x - other.x;
    point.y = y - other.y;
    point.z = z - other.z;
    return point;
}

QString Point::toString() const {

    QStringList components;
    components << QString::number(x);
    components << QString::number(y);
    components << QString::number(z);
    return "[" + components.join(", ") + "]";
}

Point Point::fromString(const QString &string) {

    if (!string.startsWith("[") || !string.endsWith("]")) {
        throw GameException(GameException::InvalidPoint);
    }

    QStringList components = string.mid(1, -1).split(',');
    if (components.length() != 3) {
        throw GameException(GameException::InvalidPoint);
    }

    Point point;
    point.x = components[0].trimmed().toInt();
    point.y = components[1].trimmed().toInt();
    point.z = components[2].trimmed().toInt();
    return point;
}

Point Point::fromVariantList(const QVariantList &variantList) {

    if (variantList.length() != 3) {
        throw GameException(GameException::InvalidPoint);
    }

    Point point;
    point.x = variantList[0].toInt();
    point.y = variantList[1].toInt();
    point.z = variantList[2].toInt();
    return point;
}

QScriptValue Point::toScriptValue(QScriptEngine *engine, const Point &point) {

    QScriptValue object = engine->newObject();
    object.setProperty("x", point.x);
    object.setProperty("y", point.y);
    object.setProperty("z", point.z);
    return object;
}

void Point::fromScriptValue(const QScriptValue &object, Point &point) {

    point.x = object.property("x").toInt32();
    point.y = object.property("y").toInt32();
    point.z = object.property("z").toInt32();
}
