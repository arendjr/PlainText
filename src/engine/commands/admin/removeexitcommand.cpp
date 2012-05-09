#include "removeexitcommand.h"


RemoveExitCommand::RemoveExitCommand(Character *character, QObject *parent) :
    AdminCommand(character, parent) {
}

RemoveExitCommand::~RemoveExitCommand() {
}

void RemoveExitCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: remove-exit <exit-name>")) {
        return;
    }

    GameObjectPtrList exits = takeObjects(currentArea()->exits());
    if (!requireUnique(exits, "There's no such exit.", "Exit is not unique.")) {
        return;
    }

    currentArea()->removeExit(exits[0]);

    character()->send(QString("Exit %1 removed.").arg(exits[0]->name()));
}
