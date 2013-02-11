#include "util.h"

#include <cmath>
#include <cstdarg>

#include <QDebug>
#include <QTextStream>
#include <QVector>
#include <QtAlgorithms>

#include "gameobjectptr.h"
#include "item.h"
#include "player.h"
#include "portal.h"
#include "room.h"
#include "scriptengine.h"


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

QString Util::joinPtrList(const GameObjectPtrList &list, int options) {

    if (list.isEmpty()) {
        return (options & Capitalized ? "Nothing" : "nothing");
    }

    return joinFancy(combinePtrList(list, options));
}

QStringList Util::combinePtrList(const GameObjectPtrList &list, int options) {

    QVector<GameObject *> objects;
    QStringList objectNames;
    QList<int> objectCounts;

    for (const GameObjectPtr &object : list) {
        int index = objectNames.indexOf(object->name());
        if (index > -1) {
            objectCounts[index]++;
        } else {
            objects.append(object.cast<GameObject *>());
            objectNames.append(object->name());
            objectCounts.append(1);
        }
    }

    QStringList strings;
    for (int i = 0; i < objects.size(); i++) {
        const GameObject *object = objects[i];

        if (objectCounts[i] > 1) {
            if (i == 0 && options & Capitalized) {
                strings.append(capitalize(writtenNumber(objectCounts[i])) + " " + object->plural());
            } else {
                strings.append(writtenNumber(objectCounts[i]) + " " + object->plural());
            }
        } else {
            bool capital = (i == 0 && options & Capitalized);
            bool useDefiniteArticle = (options & DefiniteArticles);
            bool impliedPlural = false;
            if (!useDefiniteArticle && object->isItem()) {
                const Item *item = qobject_cast<const Item *>(object);
                useDefiniteArticle = (item->flags() & ItemFlags::AlwaysUseDefiniteArticle);
                impliedPlural = (item->flags() & ItemFlags::ImpliedPlural);
            }
            if (useDefiniteArticle) {
                strings.append((capital ? "The " : "the ") + object->name());
            } else if (impliedPlural) {
                strings.append(capital ? Util::capitalize(object->name()) : object->name());
            } else {
                strings.append(object->indefiniteName(capital ? Capitalized : NoOptions));
            }
        }
    }
    return strings;
}

struct Direction {
    QString name;
    QString opposite;
    QString abbreviation;
    Vector3D vector;
};

static Direction directions[10] = {
    { QString("north"), QString("south"), QString("n"), Vector3D(0, -1, 0) },
    { QString("northeast"), QString("southwest"), QString("ne"), Vector3D(1, -1, 0) },
    { QString("east"), QString("west"), QString("e"), Vector3D(1, 0, 0) },
    { QString("southeast"), QString("northwest"), QString("se"), Vector3D(1, 1, 0) },
    { QString("south"), QString("north"), QString("s"), Vector3D(0, 1, 0) },
    { QString("southwest"), QString("northeast"), QString("sw"), Vector3D(-1, 1, 0) },
    { QString("west"), QString("east"), QString("w"), Vector3D(-1, 0, 0) },
    { QString("northwest"), QString("southeast"), QString("nw"), Vector3D(-1, -1, 0) },
    { QString("up"), QString("down"), QString("u"), Vector3D(0, 0, 1) },
    { QString("down"), QString("up"), QString("d"), Vector3D(0, 0, -1) }
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

int Util::numericPosition(const QString &position) {

    for (int i = 0; i < 20; i++) {
        if (position == ::writtenPositions[i]) {
            return i;
        }
    }

    if (position.endsWith("1st") || position.endsWith("2nd") ||
        position.endsWith("3rd") || position.endsWith("th")) {
        return position.left(position.length() - 2).toInt();
    } else {
        return 0;
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

QString Util::formatWeight(double weight) {

    return QString::number(round(weight)) + "kg";
}

QString Util::formatColumns(const QStringList &items, int options) {

    QString string;
    int length = items.length();
    int halfLength = length / 2 + length % 2;
    for (int i = 0; i < halfLength; i++) {
        QString first = items[i];
        QString second = i < length - halfLength ? items[i + halfLength] : QString("");

        if (options & Capitalized) {
            first = capitalize(first);
            second = capitalize(second);
        }
        if (options & Highlighted) {
            first = highlight(first);
            second = highlight(second);
        }

        string += "  " + first.leftJustified(30) + "  " + second + "\n";
    }
    return string;
}

QStringList Util::splitLines(const QString &string, int maxLineLength) {

    QStringList words = string.split(' ');
    QStringList lines;
    QString currentLine;
    int currentLineLength = 0;

    for (QString word : words) {
        int index;
        while ((index = word.indexOf('\n')) != -1) {
            QString left = word.left(index);
            if (!left.isEmpty()) {
                if (currentLine.length() + left.length() + 1 > maxLineLength) {
                    lines.append(currentLine);
                    currentLine = left;
                    currentLineLength = left.length();
                } else {
                    if (!currentLine.isEmpty()) {
                        currentLine += " ";
                    }
                    currentLine += left;
                    currentLineLength += left.length();
                }
            }
            lines.append(currentLine);
            currentLine = "";
            currentLineLength = 0;
            word = word.mid(index + 1);
        }
        if (word.isEmpty()) {
            continue;
        }

        int wordLength = word.length() - 4 * word.count("\x1B[");
        if (currentLineLength + wordLength + 1 > maxLineLength) {
            lines.append(currentLine);
            currentLine = word;
            currentLineLength = wordLength;
        } else {
            if (!currentLine.isEmpty()) {
                currentLine += " ";
            }
            currentLine += word;
            currentLineLength += wordLength + 1;
        }
    }
    if (!currentLine.isEmpty()) {
        lines.append(currentLine);
    }

    return lines;
}

QStringList Util::splitComponents(const QString &string) {

    QStringList components;
    QString component;
    for (int i = 1; i < string.length() - 1; i++) {
        QChar character = string[i];
        if (character == ',') {
            components.append(component);
            component.clear();
        } else if (!character.isSpace()) {
            component.append(character);
        }
    }
    components.append(component);
    return components;
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

QString Util::colorize(const QString &string, int color) {

    return "\x1B[" + QString(colorMap[color]) + "m" + string + "\x1B[0m";
}

QString Util::highlight(const QString &string) {

    return colorize(string, White);
}

QString Util::processHighlights(const QString &string) {

    static QRegExp bold("\\*([^*]+)\\*");
    QString result = string;
    int pos = 0;
    while ((pos = bold.indexIn(result)) != -1) {
        result = result.replace(pos, bold.matchedLength(), Util::highlight(bold.cap(1)));
    }
    return result;
}

QString Util::validateUserName(const QString &string) {

    QString userName;
    int numSpecialChars = 0;
    for (int i = 0; i < string.length(); i++) {
        if (string[i].isLetter()) {
            if (userName.isEmpty()) {
                userName.append(string[i].toUpper());
            } else {
                userName.append(string[i]);
                if (userName.length() >= 12) {
                    break;
                }
            }
        } else if (i > 0 && numSpecialChars == 0 && (string[i] == '\'' || string[i] == '-')) {
            userName.append(string[i]);
            numSpecialChars++;
            if (userName.length() >= 12) {
                break;
            }
        }
    }
    if (userName.endsWith('\'') || userName.endsWith('-')) {
        userName = userName.left(userName.length() - 1);
    }
    return userName;
}

bool Util::isVowel(const QChar &character) {

    return (character == 'a' || character == 'e' || character == 'i' ||
            character == 'o' || character == 'u');
}

bool Util::isDirection(const QString &string) {

    return indexOfDirection(string) > -1;
}

QString Util::opposingDirection(const QString &direction) {

    int index = indexOfDirection(direction);
    if (index > -1) {
        return directions[index].opposite;
    } else {
        return QString();
    }
}

bool Util::isDirectionAbbreviation(const QString &string) {

    return indexOfDirectionAbbreviation(string) > -1;
}

QString Util::direction(const QString &abbreviation) {

    int index = indexOfDirectionAbbreviation(abbreviation);
    if (index > -1) {
        return directions[index].name;
    } else {
        return QString();
    }
}

Vector3D Util::vectorForDirection(const QString &direction) {

    int index = indexOfDirection(direction);
    if (index > -1) {
        return directions[index].vector;
    } else {
        return Vector3D(0, 0, 0);
    }
}

QString Util::directionForVector(const Vector3D &vector) {

    if (abs(vector.z) > sqrt(vector.x * vector.x + vector.y * vector.y)) {
        if (vector.z > 0) {
            return directions[9].name;
        } else {
            return directions[8].name;
        }
    } else {
        double degrees = atan2(vector.y, vector.x) * 360.0 / TAU;
        if (degrees < -157.5 || degrees > 157.5) {
            return directions[6].name;
        } else if (degrees >= -157.5 && degrees < -112.5) {
            return directions[7].name;
        } else if (degrees >= -112.5 && degrees < -67.5) {
            return directions[0].name;
        } else if (degrees >= -67.5 && degrees < -22.5) {
            return directions[1].name;
        } else if (degrees >= -22.5 && degrees < 22.5) {
            return directions[2].name;
        } else if (degrees >= 22.5 && degrees < 67.5) {
            return directions[3].name;
        } else if (degrees >= 67.5 && degrees < 112.5) {
            return directions[4].name;
        } else {
            return directions[5].name;
        }
    }
}

double Util::angleBetweenXYVectors(const Vector3D &vector1, const Vector3D &vector2) {

    double angle = atan2(vector1.y, vector1.x) - atan2(vector2.y, vector2.x);
    if (angle < -TAU / 2) {
        angle += TAU;
    } else if (angle > TAU / 2) {
        angle -= TAU;
    }
    return angle;
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

    const QMetaObject *metaObject = object->metaObject();
    int count = metaObject->propertyCount(),
        offset = GameObject::staticMetaObject.propertyOffset();
    for (int i = offset; i < count; i++) {
        QMetaProperty metaProperty = metaObject->property(i);
        QString metaPropertyName = metaProperty.name();
        if (metaPropertyName.startsWith(name)) {
            return metaPropertyName;
        }
    }
    return QString();
}

QString Util::randomString(int length) {

    QString string;
    for (int i = 0; i < length; i++) {
        string.append(QChar::fromLatin1(40 + qrand() % 87));
    }
    return string;
}

QString Util::htmlEscape(QString string) {

    string.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;").replace("\"", "&quot;");
    return string;
}

int Util::randomInt(int min, int max) {

    if (max == min) {
        return min;
    }
    return min + qrand() % (max - min);
}

QString Util::randomAlternative(int amount, ...) {

    QStringList alternatives;

    va_list vl;
    va_start(vl, amount);
    for (int i = 0; i < amount; i++) {
        char *val = va_arg(vl, char *);
        alternatives.append(val);
    }
    va_end(vl);

    return alternatives[randomInt(0, amount)];
}
