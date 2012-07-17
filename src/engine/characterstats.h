#ifndef CHARACTERSTATS_H
#define CHARACTERSTATS_H

#include <QMetaType>
#include <QScriptValue>
#include <QString>
#include <QVariantList>


class CharacterStats {

    public:
        CharacterStats();

        int strength;
        int dexterity;
        int vitality;
        int endurance;
        int intelligence;
        int faith;

        inline int total() const {
            return strength + dexterity + vitality + endurance + intelligence + faith;
        }

        CharacterStats &operator=(const CharacterStats &other);
        bool operator==(const CharacterStats &other) const;
        bool operator!=(const CharacterStats &other) const;

        CharacterStats operator+(const CharacterStats &other) const;
        CharacterStats &operator+=(const CharacterStats &other);

        bool isNull() const;

        QString toString() const;
        static CharacterStats fromString(const QString &string);

        static CharacterStats fromVariantList(const QVariantList &variantList);

        static QScriptValue toScriptValue(QScriptEngine *engine, const CharacterStats &stats);
        static void fromScriptValue(const QScriptValue &object, CharacterStats &stats);
};

typedef QList<CharacterStats> CharacterStatsList;

Q_DECLARE_METATYPE(CharacterStats)
Q_DECLARE_METATYPE(CharacterStatsList)

#endif // CHARACTERSTATS_H
