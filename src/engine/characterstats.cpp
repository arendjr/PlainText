#include "characterstats.h"

#include <cstring>

#include <QScriptEngine>
#include <QStringList>


CharacterStats::CharacterStats() :
    strength(0),
    dexterity(0),
    vitality(0),
    endurance(0),
    intelligence(0),
    faith(0),
    height(0),
    weight(0) {
}

CharacterStats &CharacterStats::operator=(const CharacterStats &other) {

    memcpy(this, &other, sizeof(CharacterStats));
    return *this;
}

bool CharacterStats::operator==(const CharacterStats &other) const {

    return memcmp(this, &other, sizeof(CharacterStats)) == 0;
}

bool CharacterStats::operator!=(const CharacterStats &other) const {

    return memcmp(this, &other, sizeof(CharacterStats)) != 0;
}

QString CharacterStats::toString() const {

    QStringList components;
    components << QString::number(strength);
    components << QString::number(dexterity);
    components << QString::number(vitality);
    components << QString::number(endurance);
    components << QString::number(intelligence);
    components << QString::number(faith);
    components << QString::number(height);
    components << QString::number(weight);
    return "[" + components.join(", ") + "]";
}

CharacterStats CharacterStats::fromString(const QString &string) throw (GameException) {

    if (!string.startsWith("[") || !string.endsWith("]")) {
        throw GameException(GameException::InvalidCharacterStats);
    }

    QStringList components = string.mid(1, -1).split(',');
    if (components.length() != 8) {
        throw GameException(GameException::InvalidCharacterStats);
    }

    CharacterStats stats;
    stats.strength = components[0].trimmed().toInt();
    stats.dexterity = components[1].trimmed().toInt();
    stats.vitality = components[2].trimmed().toInt();
    stats.endurance = components[3].trimmed().toInt();
    stats.intelligence = components[4].trimmed().toInt();
    stats.faith = components[5].trimmed().toInt();
    stats.height = components[6].trimmed().toInt();
    stats.weight = components[7].trimmed().toInt();
    return stats;
}

CharacterStats CharacterStats::fromVariantList(const QVariantList &variantList) throw (GameException) {

    if (variantList.length() != 8) {
        throw GameException(GameException::InvalidCharacterStats);
    }

    CharacterStats stats;
    stats.strength = variantList[0].toInt();
    stats.dexterity = variantList[1].toInt();
    stats.vitality = variantList[2].toInt();
    stats.endurance = variantList[3].toInt();
    stats.intelligence = variantList[4].toInt();
    stats.faith = variantList[5].toInt();
    stats.height = variantList[6].toInt();
    stats.weight = variantList[7].toInt();
    return stats;
}

QScriptValue CharacterStats::toScriptValue(QScriptEngine *engine, const CharacterStats &stats) {

    QScriptValue object = engine->newObject();
    object.setProperty("strength", stats.strength);
    object.setProperty("dexterity", stats.dexterity);
    object.setProperty("vitality", stats.vitality);
    object.setProperty("endurance", stats.endurance);
    object.setProperty("intelligence", stats.intelligence);
    object.setProperty("faith", stats.faith);
    object.setProperty("height", stats.height);
    object.setProperty("weight", stats.weight);
    return object;
}

void CharacterStats::fromScriptValue(const QScriptValue &object, CharacterStats &stats) {

    stats.strength = object.property("strength").toInt32();
    stats.dexterity = object.property("dexterity").toInt32();
    stats.vitality = object.property("vitality").toInt32();
    stats.endurance = object.property("endurance").toInt32();
    stats.intelligence = object.property("intelligence").toInt32();
    stats.faith = object.property("faith").toInt32();
    stats.height = object.property("height").toInt32();
    stats.weight = object.property("weight").toInt32();
}
