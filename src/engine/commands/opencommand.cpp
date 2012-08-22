#include "opencommand.h"


OpenCommand::OpenCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Open an exit, typically a door. Note that doors automatically close after a "
                   "while.\n"
                   "\n"
                   "Example: open door");
}

OpenCommand::~OpenCommand() {
}

void OpenCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Open what?")) {
        return;
    }

    takeWord("the");

    GameObjectPtr exit = takeObject(currentArea()->exits());
    if (!requireSome(exit, "That's not here.")) {
        return;
    }

    player()->open(exit);
}
