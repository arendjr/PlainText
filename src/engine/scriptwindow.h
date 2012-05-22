#ifndef SCRIPTWINDOW_H
#define SCRIPTWINDOW_H

#include <QHash>
#include <QObject>
#include <QScriptValue>


class QScriptEngine;

class ScriptWindow : public QObject {

    Q_OBJECT

    public:
        explicit ScriptWindow(const QScriptValue &originalWindow, QObject *parent = 0);

        QScriptValue toScriptValue();

        Q_INVOKABLE int randomInt(int min = 0, int max = INT32_MAX) const;

        Q_INVOKABLE int setInterval(const QScriptValue &function, int delay);
        Q_INVOKABLE void clearInterval(int timerId);

        Q_INVOKABLE int setTimeout(const QScriptValue &function, int delay);
        Q_INVOKABLE void clearTimeout(int timerId);

    protected:
        virtual void timerEvent(QTimerEvent *event);

    private:
        QScriptEngine *m_engine;
        QScriptValue m_originalWindow;

        QHash<int, QScriptValue> m_intervalHash;
        QHash<int, QScriptValue> m_timeoutHash;
};

#endif // SCRIPTWINDOW_H
