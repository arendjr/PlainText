#include "characterstats.h"

#include <cstring>

#include <QScriptEngine>
#include <QStringList>

#include "gameexception.h"
#include "util.h"


CharacterStats::CharacterStats(int strength, int dexterity, int vitality,
                               int endurance, int intelligence, int faith) :
    strength(strength),
    dexterity(dexterity),
    vitality(vitality),
    endurance(endurance),
    intelligence(intelligence),
    faith(faith) {
}

bool CharacterStats::operator==(const CharacterStats &other) const {

    return memcmp(this, &other, sizeof(CharacterStats)) == 0;
}

bool CharacterStats::operator!=(const CharacterStats &other) const {

    return memcmp(this, &other, sizeof(CharacterStats)) != 0;
}

CharacterStats CharacterStats::operator+(const CharacterStats &other) const {

    CharacterStats stats;
    stats.strength = strength + other.strength;
    stats.dexterity = dexterity + other.dexterity;
    stats.vitality = vitality + other.vitality;
    stats.endurance = endurance + other.endurance;
    stats.intelligence = intelligence + other.intelligence;
    stats.faith = faith + other.faith;
    return stats;
}

CharacterStats &CharacterStats::operator+=(const CharacterStats &other) {

    strength += other.strength;
    dexterity += other.dexterity;
    vitality += other.vitality;
    endurance += other.endurance;
    intelligence += other.intelligence;
    faith += other.faith;
    return *this;
}

bool CharacterStats::isNull() const {

    return strength == 0 && dexterity == 0 && vitality == 0 &&
           endurance == 0 && intelligence == 0 && faith == 0;
}

QString CharacterStats::toString() const {

    QStringList stringList;
    stringList << QString::number(strength);
    stringList << QString::number(dexterity);
    stringList << QString::number(vitality);
    stringList << QString::number(endurance);
    stringList << QString::number(intelligence);
    stringList << QString::number(faith);
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
    if (stringList.length() != 6) {
        throw GameException(GameException::InvalidCharacterStats);
    }

    stats.strength = stringList[0].toInt();
    stats.dexterity = stringList[1].toInt();
    stats.vitality = stringList[2].toInt();
    stats.endurance = stringList[3].toInt();
    stats.intelligence = stringList[4].toInt();
    stats.faith = stringList[5].toInt();
}

QString CharacterStats::toJsonString(const CharacterStats &stats, Options options) {

    Q_UNUSED(options);

    return stats.toString();
}

void CharacterStats::fromVariant(const QVariant &variant, CharacterStats &stats) {

    QVariantList variantList = variant.toList();
    if (variantList.length() != 6) {
        throw GameException(GameException::InvalidCharacterStats);
    }

    stats.strength = variantList[0].toInt();
    stats.dexterity = variantList[1].toInt();
    stats.vitality = variantList[2].toInt();
    stats.endurance = variantList[3].toInt();
    stats.intelligence = variantList[4].toInt();
    stats.faith = variantList[5].toInt();
}

QScriptValue CharacterStats::toScriptValue(QScriptEngine *engine, const CharacterStats &stats) {

    QScriptValue object = engine->newObject();
    object.setProperty("strength", stats.strength);
    object.setProperty("dexterity", stats.dexterity);
    object.setProperty("vitality", stats.vitality);
    object.setProperty("endurance", stats.endurance);
    object.setProperty("intelligence", stats.intelligence);
    object.setProperty("faith", stats.faith);
    return object;
}

void CharacterStats::fromScriptValue(const QScriptValue &object, CharacterStats &stats) {

    stats.strength = object.property("strength").toInt32();
    stats.dexterity = object.property("dexterity").toInt32();
    stats.vitality = object.property("vitality").toInt32();
    stats.endurance = object.property("endurance").toInt32();
    stats.intelligence = object.property("intelligence").toInt32();
    stats.faith = object.property("faith").toInt32();
}
