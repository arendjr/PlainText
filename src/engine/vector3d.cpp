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

void Vector3D::fromUserString(const QString &string, Vector3D &vector) {

    if (!string.startsWith("[") || !string.endsWith("]")) {
        throw GameException(GameException::InvalidVector);
    }

    QStringList stringList = string.mid(1, string.length() - 2).split(',');
    if (stringList.length() != 3) {
        throw GameException(GameException::InvalidVector);
    }

    vector.x = stringList[0].trimmed().toInt();
    vector.y = stringList[1].trimmed().toInt();
    vector.z = stringList[2].trimmed().toInt();
}

QString Vector3D::toJsonString(const Vector3D &vector, Options options) {

    Q_UNUSED(options)

    return vector.toString();
}

void Vector3D::fromVariant(const QVariant &variant, Vector3D &vector) {

    QVariantList variantList = variant.toList();
    if (variantList.length() != 3) {
        throw GameException(GameException::InvalidVector);
    }

    vector.x = variantList[0].toInt();
    vector.y = variantList[1].toInt();
    vector.z = variantList[2].toInt();
}

QScriptValue Vector3D::toScriptValue(QScriptEngine *engine, const Vector3D &vector) {

    QScriptValue object = engine->newArray(3);
    object.setProperty(0, vector.x);
    object.setProperty(1, vector.y);
    object.setProperty(2, vector.z);
    return object;
}

void Vector3D::fromScriptValue(const QScriptValue &object, Vector3D &vector) {

    vector.x = object.property(0).toInt32();
    vector.y = object.property(1).toInt32();
    vector.z = object.property(2).toInt32();
}

Vector3D operator*(int scalar, const Vector3D &vector) {

    return Vector3D(scalar * vector.x, scalar * vector.y, scalar * vector.z);
}
