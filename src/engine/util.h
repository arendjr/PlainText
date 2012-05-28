#ifndef UTIL_H
#define UTIL_H

#include <QObject>
#include <QStringList>

#include "gameobjectptr.h"


enum Color {
    White = 0,
    Silver,
    Gray,
    Black,
    Red,
    Maroon,
    Yellow,
    Olive,
    Lime,
    Green,
    Aqua,
    Teal,
    Blue,
    Navy,
    Fuchsia,
    Purple
};

enum Articles {
    IndefiniteArticle,
    DefiniteArticle
};

class Util : public QObject {

    Q_OBJECT

    public:
        static void instantiate();
        static void destroy();

        Q_INVOKABLE static QString joinFancy(const QStringList &list,
                                             const QString &separator = ", ",
                                             const QString &last = " and ");

        static QString joinItems(const GameObjectPtrList &items, Articles article = IndefiniteArticle);

        Q_INVOKABLE static QString writtenNumber(int number);

        Q_INVOKABLE static QString capitalize(const QString &string);

        Q_INVOKABLE static QString center(const QString &string, int width);

        Q_INVOKABLE static QString formatHeight(int height);

        Q_INVOKABLE static QString formatWeight(int weight);

        static QStringList splitLines(const QString &string, int maxLineLength = 80);

        static QString jsString(QString string);

        static QString colorize(const QString &string, Color color);

        Q_INVOKABLE static QString highlight(const QString &string);

        static bool isDirection(const QString &string);

        static QString opposingDirection(const QString &direction);

        static bool isDirectionAbbreviation(const QString &string);

        static QString direction(const QString &abbreviation);

        static QString toCamelCase(QString string);

        Q_INVOKABLE static void sendOthers(const GameObjectPtrList &players, const QString &message,
                                           const GameObjectPtr &exclude1 = GameObjectPtr(),
                                           const GameObjectPtr &exclude2 = GameObjectPtr());

    private:
        static Util *s_instance;

        explicit Util();
        virtual ~Util();
};

#endif // UTIL_H
