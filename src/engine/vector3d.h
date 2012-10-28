#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <QMetaType>
#include <QScriptValue>
#include <QString>
#include <QVariantList>


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
        static Vector3D fromString(const QString &string);

        static Vector3D fromVariantList(const QVariantList &variantList);

        static QScriptValue toScriptValue(QScriptEngine *engine, const Vector3D &stats);
        static void fromScriptValue(const QScriptValue &object, Vector3D &stats);
};

Q_DECLARE_METATYPE(Vector3D)

#endif // VECTOR3D_H
