#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <QScriptValue>
#include <QString>
#include <QVariant>

#include "constants.h"
#include "metatyperegistry.h"


class Vector3D {

    public:
        int x;
        int y;
        int z;

        Vector3D() = default;
        Vector3D(int x, int y, int z);

        bool operator==(const Vector3D &other) const;
        bool operator!=(const Vector3D &other) const;

        Vector3D operator+(const Vector3D &other) const;
        Vector3D operator-(const Vector3D &other) const;

        QString toString() const;

        static QString toUserString(const Vector3D &vector);
        static Vector3D fromUserString(const QString &string);

        static QString toJsonString(const Vector3D &vector, Options options = NoOptions);
        static Vector3D fromVariant(const QVariant &variant);

        static QScriptValue toScriptValue(QScriptEngine *engine, const Vector3D &stats);
        static void fromScriptValue(const QScriptValue &object, Vector3D &stats);
};

Vector3D operator*(int scalar, const Vector3D &vector);

PT_DECLARE_SERIALIZABLE_METATYPE(Vector3D)

#endif // VECTOR3D_H
