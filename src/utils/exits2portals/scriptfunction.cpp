#include "scriptfunction.h"

#include "conversionutil.h"


ScriptFunction::ScriptFunction() {
}

ScriptFunction::ScriptFunction(const ScriptFunction &other) :
    source(other.source) {
}

ScriptFunction &ScriptFunction::operator=(const ScriptFunction &other) {

    source = other.source;
    return *this;
}

bool ScriptFunction::operator==(const ScriptFunction &other) const {

    return source == other.source;
}

bool ScriptFunction::operator!=(const ScriptFunction &other) const {

    return source != other.source;
}

QString ScriptFunction::toString() const {

    return source;
}

QString ScriptFunction::toUserString(const ScriptFunction &scriptFunction) {

    return scriptFunction.toString();
}

ScriptFunction ScriptFunction::fromUserString(const QString &string) {

    ScriptFunction function;
    function.source = string;
    return function;
}

QString ScriptFunction::toJsonString(const ScriptFunction &scriptFunction, Options options) {

    Q_UNUSED(options)

    return ConversionUtil::jsString(scriptFunction.source);
}

ScriptFunction ScriptFunction::fromVariant(const QVariant &variant) {

    ScriptFunction function;
    function.source = variant.toString();
    return function;
}

QScriptValue ScriptFunction::toScriptValue(QScriptEngine *engine, const ScriptFunction &function) {

    Q_UNUSED(engine);
    return QScriptValue(function.source);
}

void ScriptFunction::fromScriptValue(const QScriptValue &value, ScriptFunction &function) {

    function.source = value.toString();
}
