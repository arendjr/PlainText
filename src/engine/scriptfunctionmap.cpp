#include "scriptfunctionmap.h"

#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QStringList>

#include "conversionutil.h"
#include "gameexception.h"


ScriptFunctionMap::ScriptFunctionMap() :
    QMap<QString, ScriptFunction>() {
}

ScriptFunctionMap::ScriptFunctionMap(const ScriptFunctionMap &other) :
    QMap<QString, ScriptFunction>(static_cast<const QMap<QString, ScriptFunction> &>(other)) {
}

ScriptFunctionMap::ScriptFunctionMap(const QMap<QString, ScriptFunction> &other) :
    QMap<QString, ScriptFunction>(other) {
}

QString ScriptFunctionMap::toUserString(const ScriptFunctionMap &functionMap) {

    Q_UNUSED(functionMap)

    return "(function map)";
}

ScriptFunctionMap ScriptFunctionMap::fromUserString(const QString &string) {

    Q_UNUSED(string)

    throw GameException(GameException::NotSupported,
                        "Converting user strings to script function map not (yet) supported");
}

QString ScriptFunctionMap::toJsonString(const ScriptFunctionMap &functionMap, Options options) {

    Q_UNUSED(options)

    QStringList stringList;
    for (const QString &key : functionMap.keys()) {
        stringList << QString("%1: %2").arg(ConversionUtil::jsString(key),
                                            ScriptFunction::toJsonString(functionMap[key]));
    }
    return stringList.isEmpty() ? QString() : "{ " + stringList.join(", ") + " }";
}

ScriptFunctionMap ScriptFunctionMap::fromVariant(const QVariant &variant) {

    ScriptFunctionMap functionMap;
    QVariantMap variantMap = variant.toMap();
    for (const QString &key : variantMap.keys()) {
        functionMap[key] = ScriptFunction::fromVariant(variantMap[key]);
    }
    return functionMap;
}

QScriptValue ScriptFunctionMap::toScriptValue(QScriptEngine *engine, const ScriptFunctionMap &map) {

    QScriptValue value = engine->newObject();
    for (const QString &key : map.keys()) {
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
