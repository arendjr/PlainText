#ifndef SCRIPTFUNCTIONMAP_H
#define SCRIPTFUNCTIONMAP_H

#include <QMap>
#include <QString>

#include "metatyperegistry.h"
#include "scriptfunction.h"


class QScriptEngine;
class QScriptValue;

class ScriptFunctionMap : public QMap<QString, ScriptFunction> {

    public:
        ScriptFunctionMap();
        ScriptFunctionMap(const ScriptFunctionMap &other);
        ScriptFunctionMap(const QMap<QString, ScriptFunction> &other);

        static QString toUserString(const ScriptFunctionMap &functionMap);
        static ScriptFunctionMap fromUserString(const QString &string);

        static QString toJsonString(const ScriptFunctionMap &functionMap,
                                    Options options = NoOptions);
        static ScriptFunctionMap fromVariant(const QVariant &variant);

        static QScriptValue toScriptValue(QScriptEngine *engine, const ScriptFunctionMap &map);
        static void fromScriptValue(const QScriptValue &value, ScriptFunctionMap &map);
};

PT_DECLARE_SERIALIZABLE_METATYPE(ScriptFunctionMap)

#endif // SCRIPTFUNCTIONMAP_H
