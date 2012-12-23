#include "removeexitcommand.h"


#define super AdminCommand

RemoveExitCommand::RemoveExitCommand(QObject *parent) :
    super(parent) {

    setDescription("Remove an exit from the current area.\n"
                   "\n"
                   "Usage: remove-exit <exit-name> [#]");
}

RemoveExitCommand::~RemoveExitCommand() {
}

void RemoveExitCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: remove-exit <exit-name> [#]")) {
        return;
    }

    GameObjectPtrList exits = takeObjects(currentRoom()->exits());
    if (!requireUnique(exits, "There's no such exit.", "Exit is not unique.")) {
        return;
    }

    exits[0]->setDeleted();

    send(QString("Exit %1 removed.").arg(exits[0]->name()));
}
