#ifndef MODIFIER_H
#define MODIFIER_H

#include <QScriptValue>
#include <QString>

#include "characterstats.h"
#include "metatyperegistry.h"


class Modifier {

    public:
        qint64 started;
        int duration;

        CharacterStats stats;

        QString message;

        static QScriptValue toScriptValue(QScriptEngine *engine, const Modifier &modifier);
        static void fromScriptValue(const QScriptValue &object, Modifier &modifier);
};

typedef QList<Modifier> ModifierList;

PT_DECLARE_METATYPE(Modifier)
PT_DECLARE_METATYPE(ModifierList)

#endif // MODIFIER_H
