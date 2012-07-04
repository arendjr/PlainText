#include "effect.h"

#include <QScriptEngine>


Effect::Effect() :
    started(0),
    delay(0),
    numOccurrences(1),
    hpDelta(0),
    mpDelta(0) {
}

QScriptValue Effect::toScriptValue(QScriptEngine *engine, const Effect &effect) {

    QScriptValue object = engine->newObject();
    object.setProperty("delay", effect.delay);
    object.setProperty("numOccurrences", effect.numOccurrences);
    object.setProperty("hpDelta", effect.hpDelta);
    object.setProperty("mpDelta", effect.mpDelta);
    object.setProperty("message", effect.message);
    return object;
}

void Effect::fromScriptValue(const QScriptValue &object, Effect &effect) {

    effect.delay = object.property("delay").toInt32();
    effect.numOccurrences = object.property("numOccurrences").toInt32();
    effect.hpDelta = object.property("hpDelta").toInt32();
    effect.mpDelta = object.property("mpDelta").toInt32();
    effect.message = object.property("message").toString();
}
