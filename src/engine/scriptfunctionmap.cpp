#include "scriptfunctionmap.h"

#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>


ScriptFunctionMap::ScriptFunctionMap() :
    QMap<QString, ScriptFunction>() {
}

ScriptFunctionMap::ScriptFunctionMap(const ScriptFunctionMap &other) :
    QMap<QString, ScriptFunction>(static_cast<const QMap<QString, ScriptFunction> &>(other)) {
}

ScriptFunctionMap::ScriptFunctionMap(const QMap<QString, ScriptFunction> &other) :
    QMap<QString, ScriptFunction>(other) {
}

QScriptValue ScriptFunctionMap::toScriptValue(QScriptEngine *engine, const ScriptFunctionMap &map) {

    QScriptValue value = engine->newObject();
    foreach (const QString &key, map.keys()) {
        value.setProperty(key, ScriptFunction::toScriptValue(engine, map[key]));
    }
    return value;
}

void ScriptFunctionMap::fromScriptValue(const QScriptValue &value, ScriptFunctionMap &map) {

    map.clear();
    QScriptValueIterator it(value);
    while (it.hasNext()) {
        it.next();
        ScriptFunction function;
        ScriptFunction::fromScriptValue(it.value(), function);
        map[it.name()] = function;
    }
}
