#include "character.h"
#include "gameobjectptr.h"
#include "item.h"
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

QString Util::joinItems(const GameObjectPtrList &list, Articles article) {

    QList<Item *> items;
    QStringList itemNames;
    QList<int> itemCounts;

    foreach (const GameObjectPtr &itemPtr, list) {
        Item *item = itemPtr.cast<Item *>();
        Q_ASSERT(item);

        int index = itemNames.indexOf(item->name());
        if (index > -1) {
            itemCounts[index]++;
        } else {
            items << item;
            itemNames << item->name();
            itemCounts << 1;
        }
    }

    QStringList strings;
    for (int i = 0; i < items.length(); i++) {
        Item *item = items[i];

        if (itemCounts[i] > 1) {
            strings << writtenNumber(itemCounts[i]) + " " + item->plural();
        } else {
            if (article == DefiniteArticle) {
                strings << "the " + item->name();
            } else {
                strings << item->indefiniteArticle() + " " + item->name();
            }
        }
    }

    return joinFancy(strings);
}

static const char *writtenNumbers[] = {
    "zero",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "sevens",
    "eight",
    "nine",
    "ten",
    "eleven",
    "twelve",
    "thirteen",
    "fourteen",
    "fifteen",
    "sixteen",
    "seventeen",
    "eighteen",
    "nineteen"
};

QString Util::writtenNumber(int number) {

    if (number >= 0 && number <= 19) {
        return ::writtenNumbers[number];
    } else {
        return QString::number(number);
    }
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

    return "\x1B[" + QString(colorMap[color]) + "m" + string + "\x1B[0m";

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

QString Util::toCamelCase(QString string) {

    while (string.left(string.length() - 1).contains('-')) {
        int index = string.indexOf('-');
        string = string.left(index) + string[index + 1].toUpper() + string.mid(index + 2);
    }
    return string;
}

void Util::sendOthers(GameObjectPtrList characters, const QString &string,
                      const GameObjectPtr &exclude1, const GameObjectPtr &exclude2) {

    characters.removeOne(exclude1);
    characters.removeOne(exclude2);
    if (characters.length() > 0) {
        foreach (const GameObjectPtr &characterPtr, characters) {
            Character *character = characterPtr.cast<Character *>();
            Q_ASSERT(character);

            character->send(string);
        }
    }
}

Util::Util() {
}
