#ifndef CHARACTERSTATS_H
#define CHARACTERSTATS_H

#include <QMetaType>
#include <QScriptValue>
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

        static QScriptValue toScriptValue(QScriptEngine *engine, const CharacterStats &stats);
        static void fromScriptValue(const QScriptValue &object, CharacterStats &stats);
};

typedef QList<CharacterStats> CharacterStatsList;

Q_DECLARE_METATYPE(CharacterStats)
Q_DECLARE_METATYPE(CharacterStatsList)

#endif // CHARACTERSTATS_H
