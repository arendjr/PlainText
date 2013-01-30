#ifndef UTIL_H
#define UTIL_H

#include <cstdlib>

#include <QObject>
#include <QStringList>

#include "constants.h"
#include "gameobjectptr.h"
#include "vector3d.h"


#define TAU 6.2831853071


class Util : public QObject {

    Q_OBJECT

    public:
        Q_INVOKABLE static QString joinFancy(const QStringList &list,
                                             const QString &separator = ", ",
                                             const QString &last = " and ");

        Q_INVOKABLE static QString joinPtrList(const GameObjectPtrList &list,
                                               int options = NoOptions);

        Q_INVOKABLE static QStringList combinePtrList(const GameObjectPtrList &list,
                                                      int options = NoOptions);

        Q_INVOKABLE static QStringList sortExitNames(const QStringList &exitNames);

        Q_INVOKABLE static QString writtenNumber(int number);

        Q_INVOKABLE static QString writtenPosition(int position);

        Q_INVOKABLE static int numericPosition(const QString &position);

        Q_INVOKABLE static QString capitalize(const QString &string);

        Q_INVOKABLE static QString center(const QString &string, int width);

        Q_INVOKABLE static QString formatHeight(int height);

        Q_INVOKABLE static QString formatWeight(int weight);

        Q_INVOKABLE static QString formatColumns(const QStringList &items, int options = NoOptions);

        Q_INVOKABLE static QStringList splitLines(const QString &string, int maxLineLength = 80);

        Q_INVOKABLE static QStringList splitComponents(const QString &string);

        Q_INVOKABLE static QString colorize(const QString &string, int color);

        Q_INVOKABLE static QString highlight(const QString &string);

        Q_INVOKABLE static QString processHighlights(const QString &string);

        Q_INVOKABLE static QString validateUserName(const QString &string);

        Q_INVOKABLE static bool isVowel(const QChar &character);

        Q_INVOKABLE static bool isDirection(const QString &string);

        Q_INVOKABLE static QString opposingDirection(const QString &direction);

        Q_INVOKABLE static bool isDirectionAbbreviation(const QString &string);

        Q_INVOKABLE static QString direction(const QString &abbreviation);

        Q_INVOKABLE static Vector3D vectorForDirection(const QString &direction);

        Q_INVOKABLE static QString directionForVector(const Vector3D &vector);

        Q_INVOKABLE static double angleBetweenDirectionAndPosition(const Vector3D &direction,
                                                                   const Vector3D &position);

        Q_INVOKABLE static QString toCamelCase(QString string);
        Q_INVOKABLE static QString fromCamelCase(const QString &string);

        static QString fullPropertyName(QObject *object, const QString &propertyName);

        Q_INVOKABLE static QString randomString(int length);

        Q_INVOKABLE static QString htmlEscape(QString string);

        Q_INVOKABLE static int randomInt(int min = 0, int max = RAND_MAX);
};

#endif // UTIL_H
