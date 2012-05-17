#ifndef SCRIPTFUNCTIONMAP_H
#define SCRIPTFUNCTIONMAP_H

#include <QMap>
#include <QMetaType>
#include <QString>

#include "scriptfunction.h"


class QScriptEngine;
class QScriptValue;

class ScriptFunctionMap : public QMap<QString, ScriptFunction> {

    public:
        ScriptFunctionMap();
        ScriptFunctionMap(const ScriptFunctionMap &other);
        ScriptFunctionMap(const QMap<QString, ScriptFunction> &other);

        static QScriptValue toScriptValue(QScriptEngine *engine, const ScriptFunctionMap &map);
        static void fromScriptValue(const QScriptValue &value, ScriptFunctionMap &map);
};

Q_DECLARE_METATYPE(ScriptFunctionMap)

#endif // SCRIPTFUNCTIONMAP_H
