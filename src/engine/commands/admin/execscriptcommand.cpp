#include "execscriptcommand.h"

#include <QScriptValue>

#include "scriptengine.h"


ExecScriptCommand::ExecScriptCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Executes a script.\n"
                   "\n"
                   "Usage: exec-script <script>");
}

ExecScriptCommand::~ExecScriptCommand() {
}

void ExecScriptCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: exec-script <script>")) {
        return;
    }

    ScriptEngine *scriptEngine = ScriptEngine::instance();
    QScriptValue value = scriptEngine->evaluate(takeRest());
    if (scriptEngine->hasUncaughtException()) {
        send("Exception: " + value.toString());
        scriptEngine->evaluate("");
        return;
    }

    send("Result: " + value.toString());
}
