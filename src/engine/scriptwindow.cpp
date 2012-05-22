#include "scriptwindow.h"

#include <QDebug>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QTimerEvent>


ScriptWindow::ScriptWindow(const QScriptValue &originalWindow, QObject *parent) :
    QObject(parent),
    m_engine(originalWindow.engine()),
    m_originalWindow(originalWindow) {
}

QScriptValue ScriptWindow::toScriptValue() {

    QScriptValue value = m_engine->newQObject(this, QScriptEngine::QtOwnership,
                                              QScriptEngine::ExcludeSuperClassContents);
    QScriptValueIterator it(m_originalWindow);
    while (it.hasNext()) {
        it.next();
        value.setProperty(it.name(), it.value());
    }
    return value;
}

int ScriptWindow::setInterval(const QScriptValue &function, int delay) {

    if (function.isString() || function.isFunction()) {
        int timerId = startTimer(delay);
        m_intervalHash.insert(timerId, function);
        return timerId;
    }
    return -1;
}

void ScriptWindow::clearInterval(int timerId) {

    killTimer(timerId);
    m_intervalHash.remove(timerId);
}

int ScriptWindow::setTimeout(const QScriptValue &function, int delay) {

    if (function.isString() || function.isFunction()) {
        int timerId = startTimer(delay);
        m_timeoutHash.insert(timerId, function);
        return timerId;
    }
    return -1;
}

void ScriptWindow::clearTimeout(int timerId) {

    killTimer(timerId);
    m_timeoutHash.remove(timerId);
}

void ScriptWindow::timerEvent(QTimerEvent *event) {

    int id = event->timerId();
    QScriptValue function = m_intervalHash.value(id);
    if (!function.isValid()) {
        function = m_timeoutHash.value(id);
        if (function.isValid()) {
            killTimer(id);
        }
    }

    if (function.isString()) {
        m_engine->evaluate(function.toString());
    } else if (function.isFunction()) {
        function.call();
    }

    if (m_engine->hasUncaughtException()) {
        QScriptValue exception = m_engine->uncaughtException();
        qWarning() << "Exception: " + exception.toString();
    }
}
