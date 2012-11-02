#include "modifier.h"

#include <QScriptEngine>

#include "characterstats.h"


QScriptValue Modifier::toScriptValue(QScriptEngine *engine, const Modifier &modifier) {

    QScriptValue object = engine->newObject();
    object.setProperty("duration", modifier.duration);
    object.setProperty("stats", CharacterStats::toScriptValue(engine, modifier.stats));
    object.setProperty("message", modifier.message);
    return object;
}

void Modifier::fromScriptValue(const QScriptValue &object, Modifier &modifier) {

    modifier.duration = object.property("duration").toInt32();
    CharacterStats::fromScriptValue(object.property("stats"), modifier.stats);
    modifier.message = object.property("message").toString();
}
