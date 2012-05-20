#include "closecommand.h"


CloseCommand::CloseCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Close an exit, typically a door.\n"
                   "\n"
                   "Example: close door");
}

CloseCommand::~CloseCommand() {
}

void CloseCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Close what?")) {
        return;
    }

    takeWord("the");

    GameObjectPtrList exits = takeObjects(currentArea()->exits());
    if (!requireUnique(exits, "That's not here.", "Exit is not unique.")) {
        return;
    }

    player()->close(exits[0]);
}
