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

    return QString("[ %1, %2, %3 ]").arg(x).arg(y).arg(z);
}

QString Vector3D::toUserString(const Vector3D &vector) {

    return vector.toString();
}

Vector3D Vector3D::fromUserString(const QString &string) {

    if (!string.startsWith("[") || !string.endsWith("]")) {
        throw GameException(GameException::InvalidVector);
    }

    QStringList stringList = string.mid(1, string.length() - 2).split(',');
    if (stringList.length() != 3) {
        throw GameException(GameException::InvalidVector);
    }

    return Vector3D(stringList[0].trimmed().toInt(),
                    stringList[1].trimmed().toInt(),
                    stringList[2].trimmed().toInt());
}

QString Vector3D::toJsonString(const Vector3D &vector, Options options) {

    Q_UNUSED(options)

    return QString("[ %1, %2, %3 ]").arg(vector.x).arg(vector.y).arg(vector.z);
}

Vector3D Vector3D::fromVariant(const QVariant &variant) {

    QVariantList variantList = variant.toList();
    if (variantList.length() != 3) {
        throw GameException(GameException::InvalidVector);
    }

    return Vector3D(variantList[0].toInt(), variantList[1].toInt(), variantList[2].toInt());
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

Vector3D operator*(int scalar, const Vector3D &vector) {

    return Vector3D(scalar * vector.x, scalar * vector.y, scalar * vector.z);
}
