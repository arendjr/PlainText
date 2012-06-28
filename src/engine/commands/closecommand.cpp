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

    GameObjectPtr exit = takeObject(currentArea()->exits());
    if (!requireSome(exit, "That's not here.")) {
        return;
    }

    player()->close(exit);
}
