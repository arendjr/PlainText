#include "scriptfunction.h"

#include <QDebug>

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

ScriptFunction ScriptFunction::fromString(const QString &string) {

    ScriptEngine *scriptEngine = ScriptEngine::instance();
    ScriptFunction function = scriptEngine->defineFunction(string);
    if (scriptEngine->hasUncaughtException()) {
        QScriptValue exception = scriptEngine->uncaughtException();
        qWarning() << "Script Exception: " << exception.toString().toUtf8().constData() << endl
                   << "While defining function: " << string.toUtf8().constData();
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
