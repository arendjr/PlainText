#ifndef TIMEDSCRIPTEVENT_H
#define TIMEDSCRIPTEVENT_H

#include <QScriptValue>

#include "event.h"


class GameObject;
class ScriptEngine;

class TimedScriptEvent : public Event {

    public:
        TimedScriptEvent(ScriptEngine *scriptEngine, const QScriptValue &function,
                         GameObject *thisObject = 0);
        virtual ~TimedScriptEvent();

        virtual void process();

        virtual QString toString() const;

    private:
        ScriptEngine *m_scriptEngine;
        QScriptValue m_function;
        GameObject *m_thisObject;
};

#endif // TIMEDSCRIPTEVENT_H
