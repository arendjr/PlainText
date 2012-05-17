#include "scriptfunction.h"

#include "scriptengine.h"


QString ScriptFunction::toString() const {

    return source;
}

ScriptFunction ScriptFunction::fromString(const QString &string) throw (BadGameObjectException) {

    ScriptEngine *scriptEngine = ScriptEngine::instance();
    ScriptFunction function = scriptEngine->defineFunction(string);
    if (scriptEngine->hasUncaughtException()) {
        throw BadGameObjectException(BadGameObjectException::InvalidFunctionCode);
    }
    return function;
}

QScriptValue ScriptFunction::toScriptValue(QScriptEngine *engine, const ScriptFunction &function) {

    Q_UNUSED(engine);
    return QScriptValue(function.source);
}

void ScriptFunction::fromScriptValue(const QScriptValue &value, ScriptFunction &function) {

    function = ScriptEngine::instance()->defineFunction(value.toString());
}
