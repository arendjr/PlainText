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

    GameObjectPtr exit = takeObject(currentRoom()->exits());
    if (!requireSome(exit, "Open what?")) {
        return;
    }

    player->open(exit);
}
