#include "execscriptcommand.h"

#include <QScriptValue>

#include "engine/scriptengine.h"


ExecScriptCommand::ExecScriptCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {
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
        player()->send("Exception: " + value.toString());
        return;
    }

    player()->send("Result: " + value.toString());
}
