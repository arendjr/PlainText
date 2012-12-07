#include "scriptcommand.h"

#include <QDebug>
#include <QScriptValueList>

#include "player.h"
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

    setDescription(m_scriptCommand.property("description").call(m_scriptCommand).toString());
}

ScriptCommand::~ScriptCommand() {
}

void ScriptCommand::execute(Player *player, const QString &command) {

    ScriptEngine *engine = player->realm()->scriptEngine();
    QScriptValueList arguments;
    arguments.append(engine->toScriptValue(static_cast<GameObject *>(player)));
    arguments.append(command);
    m_scriptCommand.property("execute").call(m_scriptCommand, arguments);
    if (engine->hasUncaughtException()) {
        qWarning() << "Exception while executing command " << command << ": "
                   << engine->uncaughtException().toString();
    }
}
