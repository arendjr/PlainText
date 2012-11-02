#ifndef EFFECT_H
#define EFFECT_H

#include <QScriptValue>
#include <QString>

#include "metatyperegistry.h"


class Effect {

    public:
        Effect();

        qint64 started;
        int delay;
        int numOccurrences;

        int hpDelta;
        int mpDelta;

        QString message;

        static QScriptValue toScriptValue(QScriptEngine *engine, const Effect &effect);
        static void fromScriptValue(const QScriptValue &object, Effect &effect);
};

typedef QList<Effect> EffectList;

PT_DECLARE_METATYPE(Effect)
PT_DECLARE_METATYPE(EffectList)

#endif // EFFECT_H
