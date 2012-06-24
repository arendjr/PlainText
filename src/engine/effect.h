#ifndef EFFECT_H
#define EFFECT_H

#include <QMetaType>
#include <QScriptValue>
#include <QString>


class Effect {

    public:
        Effect();

        int delay;
        int numOccurrences;

        int hpDelta;
        int mpDelta;

        QString message;

        static QScriptValue toScriptValue(QScriptEngine *engine, const Effect &effect);
        static void fromScriptValue(const QScriptValue &object, Effect &effect);
};

typedef QList<Effect> EffectList;

Q_DECLARE_METATYPE(Effect)
Q_DECLARE_METATYPE(EffectList)

#endif // EFFECT_H
