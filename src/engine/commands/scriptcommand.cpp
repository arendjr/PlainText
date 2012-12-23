#include "scriptcommand.h"

#include <QDebug>
#include <QScriptValueList>

#include "realm.h"
#include "scriptengine.h"


#define super Command

ScriptCommand::ScriptCommand(const QScriptValue &object, QObject *parent) :
    super(parent),
    m_scriptCommand(object) {

    if (!m_scriptCommand.isObject()) {
        qDebug() << "Script command must be a Command object";
    } else if (!m_scriptCommand.property("execute").isFunction()) {
        qDebug() << "Script command has no valid execute() method";
    }

    setDescription(m_scriptCommand.property("description").toString());
}

ScriptCommand::~ScriptCommand() {
}

void ScriptCommand::execute(Character *character, const QString &command) {

    ScriptEngine *engine = ScriptEngine::instance();
    QScriptValueList arguments;
    arguments.append(engine->toScriptValue<GameObject *>(character));
    arguments.append(command);
    m_scriptCommand.property("execute").call(m_scriptCommand, arguments);
    if (engine->hasUncaughtException()) {
        QScriptValue exception = engine->uncaughtException();
        qWarning() << "Exception while executing command " << command << ": "
                   << exception.toString() << endl
                   << "Backtrace:" << endl
                   << exception.property("backtrace").toString().toUtf8().constData();
    }
}
