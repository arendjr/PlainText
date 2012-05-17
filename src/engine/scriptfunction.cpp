#include "scriptfunction.h"

#include "scriptengine.h"


ScriptFunction::ScriptFunction() {
}

ScriptFunction::ScriptFunction(const ScriptFunction &other) :
    value(other.value),
    source(other.source) {
}

ScriptFunction &ScriptFunction::operator=(const ScriptFunction &other) {

    value = other.value;
    source = other.source;
    return *this;
}

bool ScriptFunction::operator==(const ScriptFunction &other) const {

    return source == other.source;
}

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
