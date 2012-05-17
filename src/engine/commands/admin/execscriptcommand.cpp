#include "execscriptcommand.h"

#include <QScriptValue>

#include "engine/scriptengine.h"


ExecScriptCommand::ExecScriptCommand(Character *character, QObject *parent) :
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
        character()->send("Exception: " + value.toString());
        return;
    }

    character()->send("Result: " + value.toString());
}
