#include "characterstats.h"

#include <cstring>

#include <QScriptEngine>
#include <QStringList>

#include "gameexception.h"
#include "util.h"


CharacterStats::CharacterStats(int v) {

    for (int i = 0; i < NUM_STATS; i++) {
        value[i] = v;
    }
}

int CharacterStats::total() const {

    int sum = 0;
    for (int i = 0; i < NUM_STATS; i++) {
        sum += value[i];
    }
    return sum;
}

bool CharacterStats::operator==(const CharacterStats &other) const {

    return memcmp(this, &other, sizeof(CharacterStats)) == 0;
}

bool CharacterStats::operator!=(const CharacterStats &other) const {

    return memcmp(this, &other, sizeof(CharacterStats)) != 0;
}

CharacterStats CharacterStats::operator+(const CharacterStats &other) const {

    CharacterStats stats;
    for (int i = 0; i < NUM_STATS; i++) {
        stats.value[i] = value[i] + other.value[i];
    }
    return stats;
}

CharacterStats &CharacterStats::operator+=(const CharacterStats &other) {

    for (int i = 0; i < NUM_STATS; i++) {
        value[i] += other.value[i];
    }
    return *this;
}

bool CharacterStats::isNull() const {

    for (int i = 0; i < NUM_STATS; i++) {
        if (value[i] != 0) {
            return false;
        }
    }
    return true;
}

QString CharacterStats::toString() const {

    QStringList stringList;
    for (int i = 0; i < NUM_STATS; i++) {
        stringList.append(QString::number(value[i]));
    }
    return "[" + stringList.join(", ") + "]";
}

QString CharacterStats::toUserString(const CharacterStats &stats) {

    return stats.toString();
}

void CharacterStats::fromUserString(const QString &string, CharacterStats &stats) {

    if (!string.startsWith("[") || !string.endsWith("]")) {
        throw GameException(GameException::InvalidCharacterStats);
    }

    QStringList stringList = Util::splitComponents(string);
    if (stringList.length() != NUM_STATS) {
        throw GameException(GameException::InvalidCharacterStats);
    }

    for (int i = 0; i < NUM_STATS; i++) {
        stats.value[i] = stringList[i].toInt();
    }
}

QString CharacterStats::toJsonString(const CharacterStats &stats, Options options) {

    Q_UNUSED(options);

    return stats.toString();
}

void CharacterStats::fromVariant(const QVariant &variant, CharacterStats &stats) {

    QVariantList variantList = variant.toList();
    if (variantList.length() != NUM_STATS) {
        throw GameException(GameException::InvalidCharacterStats);
    }

    for (int i = 0; i < NUM_STATS; i++) {
        stats.value[i] = variantList[i].toInt();
    }
}

QScriptValue CharacterStats::toScriptValue(QScriptEngine *engine, const CharacterStats &stats) {

    QScriptValue object = engine->newArray(NUM_STATS);
    for (int i = 0; i < NUM_STATS; i++) {
        object.setProperty(i, stats.value[i]);
    }
    return object;
}

void CharacterStats::fromScriptValue(const QScriptValue &object, CharacterStats &stats) {

    for (int i = 0; i < NUM_STATS; i++) {
        stats.value[i] = object.property(i).toInt32();
    }
}
