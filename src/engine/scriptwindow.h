#ifndef SCRIPTWINDOW_H
#define SCRIPTWINDOW_H

#include <QObject>
#include <QScriptValue>


class QScriptEngine;

class ScriptWindow : public QObject {

    Q_OBJECT

    public:
        explicit ScriptWindow(const QScriptValue &originalWindow, QObject *parent = 0);
        virtual ~ScriptWindow();

        QScriptValue toScriptValue();

        Q_INVOKABLE int randomInt(int min = 0, int max = 2147483647) const;

    private:
        QScriptEngine *m_engine;
        QScriptValue m_originalWindow;
};

#endif // SCRIPTWINDOW_H
