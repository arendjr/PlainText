#include "execscriptcommand.h"

#include <QScriptValue>

#include "scriptengine.h"


#define super AdminCommand

ExecScriptCommand::ExecScriptCommand(QObject *parent) :
    super(parent) {

    setDescription("Executes a script.\n"
                   "\n"
                   "Usage: exec-script <script>");
}

ExecScriptCommand::~ExecScriptCommand() {
}

void ExecScriptCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: exec-script <script>")) {
        return;
    }

    ScriptEngine *scriptEngine = ScriptEngine::instance();
    QScriptValue value = scriptEngine->evaluate(takeRest());
    if (scriptEngine->hasUncaughtException()) {
        send("Exception: " + scriptEngine->uncaughtException().toString());
        return;
    }

    send("Result: " + value.toString());
}
