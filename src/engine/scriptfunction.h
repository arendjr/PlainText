#ifndef SCRIPTFUNCTION_H
#define SCRIPTFUNCTION_H

#include <QScriptValue>

#include "constants.h"
#include "metatyperegistry.h"


class QScriptEngine;

class ScriptFunction {

    public:
        QScriptValue value;
        QString source;

        ScriptFunction();
        ScriptFunction(const char *string);
        ScriptFunction(const QString &string);
        ScriptFunction(const ScriptFunction &other);

        ScriptFunction &operator=(const ScriptFunction &other);
        bool operator==(const ScriptFunction &other) const;
        bool operator!=(const ScriptFunction &other) const;

        QString toString() const;

        static QString toUserString(const ScriptFunction &scriptFunction);
        static void fromUserString(const QString &string, ScriptFunction &function);

        static QString toJsonString(const ScriptFunction &scriptFunction,
                                    Options options = NoOptions);
        static void fromVariant(const QVariant &variant, ScriptFunction &function);

        static QScriptValue toScriptValue(QScriptEngine *engine, const ScriptFunction &function);
        static void fromScriptValue(const QScriptValue &value, ScriptFunction &function);
};

PT_DECLARE_SERIALIZABLE_METATYPE(ScriptFunction)

#endif // SCRIPTFUNCTION_H
