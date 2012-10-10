#include "closecommand.h"


#define super Command

CloseCommand::CloseCommand(QObject *parent) :
    super(parent) {

    setDescription("Close an exit, typically a door.\n"
                   "\n"
                   "Example: close door");
}

CloseCommand::~CloseCommand() {
}

void CloseCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Close what?")) {
        return;
    }

    takeWord("the");

    GameObjectPtr exit = takeObject(currentRoom()->exits());
    if (!requireSome(exit, "That's not here.")) {
        return;
    }

    player->close(exit);
}
