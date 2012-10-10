#include "opencommand.h"


#define super Command

OpenCommand::OpenCommand(QObject *parent) :
    super(parent) {

    setDescription("Open an exit, typically a door. Note that doors automatically close after a "
                   "while.\n"
                   "\n"
                   "Example: open door");
}

OpenCommand::~OpenCommand() {
}

void OpenCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Open what?")) {
        return;
    }

    takeWord("the");

    GameObjectPtr exit = takeObject(currentRoom()->exits());
    if (!requireSome(exit, "That's not here.")) {
        return;
    }

    player->open(exit);
}
