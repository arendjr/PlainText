#include "removeexitcommand.h"


RemoveExitCommand::RemoveExitCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Remove an exit from the current area.\n"
                   "\n"
                   "Usage: remove-exit <exit-name> [#]");
}

RemoveExitCommand::~RemoveExitCommand() {
}

void RemoveExitCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: remove-exit <exit-name> [#]")) {
        return;
    }

    GameObjectPtrList exits = takeObjects(currentArea()->exits());
    if (!requireUnique(exits, "There's no such exit.", "Exit is not unique.")) {
        return;
    }

    exits[0]->setDeleted();

    player()->send(QString("Exit %1 removed.").arg(exits[0]->name()));
}
