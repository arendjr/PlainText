#include "util.h"

#include "gameobjectptr.h"
#include "player.h"
#include "scriptengine.h"

#include <QTextStream>
#include <QtAlgorithms>


QString Util::joinFancy(const QStringList &list, const QString &separator, const QString &last) {

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

struct Direction {
    QString name;
    QString opposite;
    QString abbreviation;
};

static Direction directions[10] = {
    { QString("north"), QString("south"), QString("n") },
    { QString("northeast"), QString("southwest"), QString("ne") },
    { QString("east"), QString("west"), QString("e") },
    { QString("southeast"), QString("northwest"), QString("se") },
    { QString("south"), QString("north"), QString("s") },
    { QString("southwest"), QString("northeast"), QString("sw") },
    { QString("west"), QString("east"), QString("w") },
    { QString("northwest"), QString("southeast"), QString("nw") },
    { QString("up"), QString("down"), QString("u") },
    { QString("down"), QString("up"), QString("d") }
};

static int indexOfDirection(const QString &direction) {

    for (int i = 0; i < 10; i++) {
        if (directions[i].name == direction) {
            return i;
        }
    }

    return -1;
}

static int indexOfDirectionAbbreviation(const QString &abbreviation) {

    for (int i = 0; i < 10; i++) {
        if (directions[i].abbreviation == abbreviation) {
            return i;
        }
    }

    return -1;
}

static bool exitNameLessThan(const QString &s1, const QString &s2) {

    int index1 = indexOfDirection(s1);
    int index2 = indexOfDirection(s2);

    if (index1 > -1) {
        if (index2 > -1) {
            return index1 < index2;
        } else {
            return true;
        }
    } else {
        if (index2 > -1) {
            return false;
        } else {
            return s1.toLower() < s2.toLower();
        }
    }
}

QStringList Util::sortExitNames(const QStringList &_exitNames) {

    QStringList exitNames = _exitNames;
    qStableSort(exitNames.begin(), exitNames.end(), exitNameLessThan);
    return exitNames;
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

static const char *writtenPositions[] = {
    "zeroth",
    "first",
    "second",
    "third",
    "fourth",
    "fifth",
    "sixth",
    "seventh",
    "eightth",
    "ninth",
    "tenth",
    "eleventh",
    "twelfth",
    "thirteenth",
    "fourteenth",
    "fifteenth",
    "sixteenth",
    "seventeenth",
    "eighteenth",
    "nineteenth"
};

QString Util::writtenPosition(int position) {

    if (position >= 1 && position <= 19) {
        return ::writtenPositions[position];
    } else {
        int rest = position % 10;
        return QString::number(position) +
               (rest == 1 ? "st" : rest == 2 ? "nd" : rest == 3 ? "rd" : "th");
    }
}

QString Util::capitalize(const QString &string) {

    return string.left(1).toUpper() + string.mid(1);
}

QString Util::center(const QString &string, int width) {

    int padding = qMax(width - string.length(), 0);
    int rightPad = padding / 2;
    int leftPad = padding - rightPad;

    return QString(leftPad, ' ') + string + QString(rightPad, ' ');
}

QString Util::formatHeight(int height) {

    QString result;
    QTextStream stream(&result);
    stream.setRealNumberPrecision(2);
    stream << ((qreal) height) / 100 << "m";
    return result;
}

QString Util::formatWeight(int weight) {

    return QString::number(weight) + "kg";
}

QStringList Util::splitLines(const QString &string, int maxLineLength) {

    QStringList words = string.split(' ');
    QStringList lines;
    QString currentLine;

    for (QString word : words) {
        int index;
        while ((index = word.indexOf('\n')) != -1) {
            QString left = word.left(index);
            if (!left.isEmpty()) {
                if (currentLine.length() + left.length() + 1 > maxLineLength) {
                    lines << currentLine;
                    currentLine = left;
                } else {
                    if (!currentLine.isEmpty()) {
                        currentLine += " ";
                    }
                    currentLine += left;
                }
            }
            lines << currentLine;
            currentLine = "";
            word = word.mid(index + 1);
        }
        if (word.isEmpty()) {
            continue;
        }

        if (currentLine.length() + word.length() + 1 > maxLineLength) {
            lines << currentLine;
            currentLine = word;
        } else {
            if (!currentLine.isEmpty()) {
                currentLine += " ";
            }
            currentLine += word;
        }
    }
    if (!currentLine.isEmpty()) {
        lines << currentLine;
    }

    return lines;
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

QString Util::highlight(const QString &string) {

    return colorize(string, White);
}

bool Util::isDirection(const QString &string) {

    return indexOfDirection(string) > -1;
}

QString Util::opposingDirection(const QString &direction) {

    Q_ASSERT(isDirection(direction));
    return directions[indexOfDirection(direction)].opposite;
}

bool Util::isDirectionAbbreviation(const QString &string) {

    return indexOfDirectionAbbreviation(string) > -1;
}

QString Util::direction(const QString &abbreviation) {

    Q_ASSERT(isDirectionAbbreviation(abbreviation));
    return directions[indexOfDirectionAbbreviation(abbreviation)].name;
}

QString Util::toCamelCase(QString string) {

    while (string.left(string.length() - 1).contains('-')) {
        int index = string.indexOf('-');
        string = string.left(index) + string[index + 1].toUpper() + string.mid(index + 2);
    }
    return string;
}

QString Util::fromCamelCase(const QString &string) {

    QString result;
    for (int i = 0; i < string.length(); i++) {
        if (string[i].isUpper()) {
            if (i == 0) {
                result += string[i].toLower();
            } else {
                result += "-" + string[i].toLower();
            }
        } else {
            result += string[i];
        }
    }
    return result;
}

QString Util::fullPropertyName(QObject *object, const QString &propertyName) {

    QString name = Util::toCamelCase(propertyName);

    QString fullPropertyName;
    const QMetaObject *metaObject = object->metaObject();
    int count = metaObject->propertyCount(),
        offset = GameObject::staticMetaObject.propertyOffset();
    for (int i = offset; i < count; i++) {
        QMetaProperty metaProperty = metaObject->property(i);
        QString metaPropertyName = metaProperty.name();
        if (metaPropertyName.startsWith(name)) {
            if (fullPropertyName.isEmpty()) {
                fullPropertyName = metaPropertyName;
            } else {
                fullPropertyName = "not unique";
                break;
            }
        }
    }
    return fullPropertyName;
}

QString Util::randomString(int length) {

    QString string;
    for (int i = 0; i < length; i++) {
        string.append(QChar::fromAscii(40 + qrand() % 87));
    }
    return string;
}
