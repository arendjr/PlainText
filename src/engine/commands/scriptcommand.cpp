#include "scriptcommand.h"

#include <QScriptValueList>

#include "logutil.h"
#include "realm.h"
#include "scriptengine.h"


#define super Command

ScriptCommand::ScriptCommand(const QScriptValue &object, QObject *parent) :
    super(parent),
    m_scriptCommand(object) {

    if (!m_scriptCommand.isObject()) {
        LogUtil::logDebug("Script command must be a Command object");
    } else if (!m_scriptCommand.property("execute").isFunction()) {
        LogUtil::logDebug("Script command has no valid execute() method");
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
        LogUtil::logException("Script Exception: %1\n"
                              "While executing command: %2",
                              engine->uncaughtException(), command);
    }
}
