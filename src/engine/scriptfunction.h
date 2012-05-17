#ifndef SCRIPTFUNCTION_H
#define SCRIPTFUNCTION_H

#include <QMetaType>
#include <QScriptValue>

#include "badgameobjectexception.h"


class QScriptEngine;

class ScriptFunction {

    public:
        QScriptValue value;
        QString source;

        ScriptFunction();
        ScriptFunction(const ScriptFunction &other);

        ScriptFunction &operator=(const ScriptFunction &other);
        bool operator==(const ScriptFunction &other) const;

        QString toString() const;

        static ScriptFunction fromString(const QString &string) throw (BadGameObjectException);

        static QScriptValue toScriptValue(QScriptEngine *engine, const ScriptFunction &function);
        static void fromScriptValue(const QScriptValue &value, ScriptFunction &function);
};

Q_DECLARE_METATYPE(ScriptFunction)

#endif // SCRIPTFUNCTION_H
