#include "vector3d.h"

#include <QScriptEngine>
#include <QStringList>

#include "gameexception.h"


Vector3D::Vector3D(int x, int y, int z) :
    x(x),
    y(y),
    z(z) {
}

bool Vector3D::operator==(const Vector3D &other) const {

    return other.x == x && other.y == y && other.z == z;
}

bool Vector3D::operator!=(const Vector3D &other) const {

    return other.x != x || other.y != y || other.z != z;
}

Vector3D Vector3D::operator+(const Vector3D &other) const {

    return Vector3D(x + other.x, y + other.y, z + other.z);
}

Vector3D Vector3D::operator-(const Vector3D &other) const {

    return Vector3D(x - other.x, y - other.y, z - other.z);
}

QString Vector3D::toString() const {

    QStringList components;
    components << QString::number(x);
    components << QString::number(y);
    components << QString::number(z);
    return "[" + components.join(", ") + "]";
}

Vector3D Vector3D::fromString(const QString &string) {

    if (!string.startsWith("[") || !string.endsWith("]")) {
        throw GameException(GameException::InvalidVector);
    }

    QStringList components = string.mid(1, -1).split(',');
    if (components.length() != 3) {
        throw GameException(GameException::InvalidVector);
    }

    Vector3D vector;
    vector.x = components[0].trimmed().toInt();
    vector.y = components[1].trimmed().toInt();
    vector.z = components[2].trimmed().toInt();
    return vector;
}

Vector3D Vector3D::fromVariantList(const QVariantList &variantList) {

    if (variantList.length() != 3) {
        throw GameException(GameException::InvalidVector);
    }

    Vector3D vector;
    vector.x = variantList[0].toInt();
    vector.y = variantList[1].toInt();
    vector.z = variantList[2].toInt();
    return vector;
}

QScriptValue Vector3D::toScriptValue(QScriptEngine *engine, const Vector3D &vector) {

    QScriptValue object = engine->newObject();
    object.setProperty("x", vector.x);
    object.setProperty("y", vector.y);
    object.setProperty("z", vector.z);
    return object;
}

void Vector3D::fromScriptValue(const QScriptValue &object, Vector3D &vector) {

    vector.x = object.property("x").toInt32();
    vector.y = object.property("y").toInt32();
    vector.z = object.property("z").toInt32();
}
