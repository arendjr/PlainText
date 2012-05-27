#include "characterstats.h"

#include <cstring>

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
