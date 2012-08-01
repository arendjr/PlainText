#ifndef SCRIPTWINDOW_H
#define SCRIPTWINDOW_H

#include <cstdlib>

#include <QObject>
#include <QScriptValue>


class QScriptEngine;

class ScriptWindow : public QObject {

    Q_OBJECT

    public:
        ScriptWindow(const QScriptValue &originalWindow, QObject *parent = 0);
        virtual ~ScriptWindow();

        QScriptValue toScriptValue();

        Q_INVOKABLE int randomInt(int min = 0, int max = RAND_MAX) const;

    private:
        QScriptEngine *m_engine;
        QScriptValue m_originalWindow;
};

#endif // SCRIPTWINDOW_H
