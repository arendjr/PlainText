#include "scriptwindow.h"

#include <QDebug>
#include <QScriptEngine>
#include <QScriptValueIterator>


ScriptWindow::ScriptWindow(const QScriptValue &originalWindow, QObject *parent) :
    QObject(parent),
    m_engine(originalWindow.engine()),
    m_originalWindow(originalWindow) {
}

ScriptWindow::~ScriptWindow() {
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

int ScriptWindow::randomInt(int min, int max) const {

    if (max == min) {
        qWarning() << "Division by zero in ScriptWindow::randomInt()";
        return min;
    }
    return min + qrand() % (max - min);
}
