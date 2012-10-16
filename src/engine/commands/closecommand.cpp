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

    GameObjectPtr exit = takeObject(currentRoom()->exits());
    if (!requireSome(exit, "Close what?")) {
        return;
    }

    player->close(exit);
}
