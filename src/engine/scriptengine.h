#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
#include <QVariantList>

#include "gameobjectptr.h"
#include "scriptfunction.h"


class MetaTypeRegistry;
class Room;
class Player;
class Session;

class ScriptEngine : public QObject {

    Q_OBJECT

    public:
        ScriptEngine();
        virtual ~ScriptEngine();

        static ScriptEngine *instance();

        void loadScripts();
        void loadScripts(const QString &dirPath);
        void loadScript(const QString &path);

        QScriptValue evaluate(const QString &program,
                              const QString &fileName = QString(), int lineNumber = 1);
        ScriptFunction defineFunction(const QString &program,
                                      const QString &fileName = QString(), int lineNumber = 1);

        bool hasUncaughtException() const;
        QScriptValue uncaughtException();

        QScriptValue executeFunction(ScriptFunction &function, const GameObjectPtr &thisObject,
                                     const QScriptValueList &arguments);

        QScriptValue toScriptValue(GameObject *object);
        QScriptValue toScriptValue(const GameObjectPtr &object);
        template <class T> QScriptValue toScriptValue(T object) {
            return m_jsEngine.toScriptValue(object);
        }

        void setGlobalObject(const char *name, QObject *object);
        void unsetGlobalObject(const char *name);

    private:
        QScriptEngine m_jsEngine;
};

#endif // SCRIPTENGINE_H
