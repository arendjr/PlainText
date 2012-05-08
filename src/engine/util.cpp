#include "util.h"

#include <QMap>


QString Util::joinFancy(const QStringList &list,
                        const QString &separator, const QString &last) {

    QString string;
    int length = list.length();
    for (int i = 0; i < length; i++) {
        string += list[i];
        if (i < length - 2) {
            string += separator;
        } else if (i == length - 2) {
            string += last;
        }
    }
    return string;
}

QString Util::jsString(QString string) {

    return "\"" + string.replace('\\', "\\\\")
                        .replace('"', "\\\"")
                        .replace('\n', "\\n") + "\"";
}

static const char *colorMap[] = {
    "37;1", "37",
    "30;1", "30",
    "31;1", "31",
    "33;1", "33",
    "32;1", "32",
    "36;1", "36",
    "34;1", "34",
    "35;1", "35"
};

QString Util::colorize(const QString &string, Color color) {

    return "\\x1B[" + QString(colorMap[color]) + "m" + string + "\\x1B[0m";

}

static QMap<QString, QString> opposingDirectionsMap;

static void initOpposingDirectionsMap() {

    if (!opposingDirectionsMap.isEmpty()) {
        return;
    }

    opposingDirectionsMap.insert("north", "south");
    opposingDirectionsMap.insert("northeast", "southwest");
    opposingDirectionsMap.insert("east", "west");
    opposingDirectionsMap.insert("southeast", "northwest");
    opposingDirectionsMap.insert("south", "north");
    opposingDirectionsMap.insert("southwest", "northeast");
    opposingDirectionsMap.insert("west", "east");
    opposingDirectionsMap.insert("northwest", "southeast");
    opposingDirectionsMap.insert("up", "down");
    opposingDirectionsMap.insert("down", "up");
}

bool Util::isDirection(const QString &string) {

    initOpposingDirectionsMap();
    return opposingDirectionsMap.contains(string);
}

QString Util::opposingDirection(const QString &direction) {

    initOpposingDirectionsMap();
    return opposingDirectionsMap[direction];
}

static QMap<QString, QString> directionAbbreviationsMap;

static void initDirectionAbbreviationsMap() {

    if (!directionAbbreviationsMap.isEmpty()) {
        return;
    }

    directionAbbreviationsMap.insert("n", "north");
    directionAbbreviationsMap.insert("ne", "northeast");
    directionAbbreviationsMap.insert("e", "east");
    directionAbbreviationsMap.insert("se", "southeast");
    directionAbbreviationsMap.insert("s", "south");
    directionAbbreviationsMap.insert("sw", "southwest");
    directionAbbreviationsMap.insert("w", "west");
    directionAbbreviationsMap.insert("nw", "northwest");
    directionAbbreviationsMap.insert("u", "up");
    directionAbbreviationsMap.insert("d", "down");
}

bool Util::isDirectionAbbreviation(const QString &string) {

    initDirectionAbbreviationsMap();
    return directionAbbreviationsMap.contains(string);
}

QString Util::direction(const QString &abbreviation) {

    initDirectionAbbreviationsMap();
    return directionAbbreviationsMap[abbreviation];
}

Util::Util() {
}
