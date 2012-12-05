#include "vector3d.h"

#include <cmath>

#include <QScriptEngine>
#include <QStringList>

#include "gameexception.h"
#include "util.h"


Vector3D::Vector3D(int x, int y, int z) :
    x(x),
    y(y),
    z(z) {
}

bool Vector3D::isNull() const {

    return x == 0 && y == 0 && z == 0;
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

int Vector3D::length() const {

    return sqrt(x * x + y * y + z * z);
}

Vector3D Vector3D::normalized() const {

    int l = length();
    Vector3D result;
    if (l > 0) {
        result.x = 100 * x / l;
        result.y = 100 * y / l;
        result.z = 100 * z / l;
    } else {
        result.x = 0;
        result.y = 0;
        result.z = 0;
    }
    return result;
}

int Vector3D::operator*(const Vector3D &other) const {

    return x * other.x +
           y * other.y +
           z * other.z;
}

double Vector3D::angle(const Vector3D &other) const {

    return acos((x * other.x + y * other.y + z * other.z) /
                (sqrt(x * x + y * y + z * z) *
                 sqrt(other.x * other.x + other.y * other.y + other.z * other.z)));
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

    QStringList stringList = Util::splitComponents(string);
    if (stringList.length() != 3) {
        throw GameException(GameException::InvalidVector);
    }

    vector.x = stringList[0].toInt();
    vector.y = stringList[1].toInt();
    vector.z = stringList[2].toInt();
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
