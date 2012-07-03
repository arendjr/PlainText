#include "timedscriptevent.h"

#include <QDebug>

#include "scriptengine.h"


TimedScriptEvent::TimedScriptEvent(ScriptEngine *scriptEngine,
                                   const QScriptValue &function, GameObject *thisObject) :
    Event(),
    m_scriptEngine(scriptEngine),
    m_function(function),
    m_thisObject(thisObject) {
}

TimedScriptEvent::~TimedScriptEvent() {
}

void TimedScriptEvent::process() {

    if (!m_scriptEngine || !m_function.isValid()) {
        qWarning() << "Processing uninitialized timed script event. Skipped.";
        return;
    }

    if (m_function.isString()) {
        m_scriptEngine->evaluate(m_function.toString());
    } else if (m_function.isFunction()) {
        if (m_thisObject) {
            m_function.call(m_scriptEngine->toScriptValue(m_thisObject));
        } else {
            m_function.call();
        }
    }

    if (m_scriptEngine->hasUncaughtException()) {
        QScriptValue exception = m_scriptEngine->uncaughtException();
        qWarning() << "Script Exception: " << exception.toString().toUtf8().constData() << endl
                   << "While executing function: " << m_function.toString().toUtf8().constData();
        m_scriptEngine->evaluate("");
    }
}

QString TimedScriptEvent::toString() const {

    return "Timer: " + m_function.toString();
}
