#include "scriptfunction.h"

#include "conversionutil.h"
#include "logutil.h"
#include "scriptengine.h"


ScriptFunction::ScriptFunction() {
}

ScriptFunction::ScriptFunction(const char *string) {

    fromUserString(QString::fromUtf8(string), *this);
}

ScriptFunction::ScriptFunction(const QString &string) {

    fromUserString(string, *this);
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

void ScriptFunction::fromUserString(const QString &string, ScriptFunction &function) {

    ScriptEngine *scriptEngine = ScriptEngine::instance();
    function = scriptEngine->defineFunction(string);
    if (scriptEngine->hasUncaughtException()) {
        LogUtil::logException("Script Exception: %1\n"
                              "While defining function: %2",
                              scriptEngine->uncaughtException(), string);
        throw GameException(GameException::InvalidFunctionCode);
    }
}

QString ScriptFunction::toJsonString(const ScriptFunction &scriptFunction, Options options) {

    Q_UNUSED(options)

    return ConversionUtil::jsString(scriptFunction.source);
}

void ScriptFunction::fromVariant(const QVariant &variant, ScriptFunction &function) {

    QString string = variant.toString();
    ScriptEngine *scriptEngine = ScriptEngine::instance();
    function = scriptEngine->defineFunction(string);
    if (scriptEngine->hasUncaughtException()) {
        LogUtil::logException("Script Exception: %1\n"
                              "While defining function: %2",
                              scriptEngine->uncaughtException(), string);
        throw GameException(GameException::InvalidFunctionCode);
    }
}

QScriptValue ScriptFunction::toScriptValue(QScriptEngine *engine, const ScriptFunction &function) {

    Q_UNUSED(engine);
    return QScriptValue(function.source);
}

void ScriptFunction::fromScriptValue(const QScriptValue &value, ScriptFunction &function) {

    function = ScriptEngine::instance()->defineFunction(value.toString());
}
