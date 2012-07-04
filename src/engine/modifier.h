#ifndef MODIFIER_H
#define MODIFIER_H

#include <QMetaType>
#include <QScriptValue>
#include <QString>

#include "characterstats.h"


class Modifier {

    public:
        Modifier();

        qint64 started;
        int duration;

        CharacterStats stats;

        QString message;

        static QScriptValue toScriptValue(QScriptEngine *engine, const Modifier &modifier);
        static void fromScriptValue(const QScriptValue &object, Modifier &modifier);
};

typedef QList<Modifier> ModifierList;

Q_DECLARE_METATYPE(Modifier)
Q_DECLARE_METATYPE(ModifierList)

#endif // MODIFIER_H
