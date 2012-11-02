#include "scriptfunction.h"

#include <QDebug>

#include "conversionutil.h"
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

    ScriptEngine *scriptEngine = ScriptEngine::instance();
    ScriptFunction function = scriptEngine->defineFunction(string);
    if (scriptEngine->hasUncaughtException()) {
        QScriptValue exception = scriptEngine->uncaughtException();
        qWarning() << "Script Exception: " << exception.toString() << endl
                   << "While defining function: " << string;
        scriptEngine->evaluate("");
        throw GameException(GameException::InvalidFunctionCode);
    }
    return function;
}

QString ScriptFunction::toJsonString(const ScriptFunction &scriptFunction, Options options) {

    Q_UNUSED(options)

    return ConversionUtil::jsString(scriptFunction.source);
}

ScriptFunction ScriptFunction::fromVariant(const QVariant &variant) {

    QString string = variant.toString();
    ScriptEngine *scriptEngine = ScriptEngine::instance();
    ScriptFunction function = scriptEngine->defineFunction(string);
    if (scriptEngine->hasUncaughtException()) {
        QScriptValue exception = scriptEngine->uncaughtException();
        qWarning() << "Script Exception: " << exception.toString() << endl
                   << "While defining function: " << string;
        scriptEngine->evaluate("");
        throw GameException(GameException::InvalidFunctionCode);
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
