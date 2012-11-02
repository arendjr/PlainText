#ifndef CHARACTERSTATS_H
#define CHARACTERSTATS_H

#include <QScriptValue>
#include <QString>
#include <QVariant>

#include "constants.h"
#include "metatyperegistry.h"


class CharacterStats {

    public:
        int strength;
        int dexterity;
        int vitality;
        int endurance;
        int intelligence;
        int faith;

        CharacterStats() = default;
        CharacterStats(int strength, int dexterity, int vitality,
                       int endurance, int intelligence, int faith);

        inline int total() const {
            return strength + dexterity + vitality + endurance + intelligence + faith;
        }

        bool operator==(const CharacterStats &other) const;
        bool operator!=(const CharacterStats &other) const;

        CharacterStats operator+(const CharacterStats &other) const;
        CharacterStats &operator+=(const CharacterStats &other);

        bool isNull() const;

        QString toString() const;

        static QString toUserString(const CharacterStats &stats);
        static CharacterStats fromUserString(const QString &string);

        static QString toJsonString(const CharacterStats &stats, Options options = NoOptions);
        static CharacterStats fromVariant(const QVariant &variant);

        static QScriptValue toScriptValue(QScriptEngine *engine, const CharacterStats &stats);
        static void fromScriptValue(const QScriptValue &object, CharacterStats &stats);
};

typedef QList<CharacterStats> CharacterStatsList;

PT_DECLARE_SERIALIZABLE_METATYPE(CharacterStats)

#endif // CHARACTERSTATS_H
