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

        bool operator==(const ScriptFunction &other) const { return source == other.source; }

        QString toString() const;

        static ScriptFunction fromString(const QString &string) throw (BadGameObjectException);

        static QScriptValue toScriptValue(QScriptEngine *engine, const ScriptFunction &function);
        static void fromScriptValue(const QScriptValue &value, ScriptFunction &function);
};

Q_DECLARE_METATYPE(ScriptFunction)

#endif // SCRIPTFUNCTION_H
