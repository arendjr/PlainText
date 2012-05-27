#ifndef CHARACTERSTATS_H
#define CHARACTERSTATS_H

#include <QMetaType>
#include <QString>
#include <QVariantList>

#include "gameexception.h"


class CharacterStats {

    public:
        CharacterStats();

        int strength;
        int dexterity;
        int vitality;
        int endurance;
        int intelligence;
        int faith;

        int height;
        int weight;

        CharacterStats &operator=(const CharacterStats &other);
        bool operator==(const CharacterStats &other) const;
        bool operator!=(const CharacterStats &other) const;

        QString toString() const;
        static CharacterStats fromString(const QString &string) throw (GameException);

        static CharacterStats fromVariantList(const QVariantList &variantList) throw (GameException);
};

Q_DECLARE_METATYPE(CharacterStats)

#endif // CHARACTERSTATS_H
