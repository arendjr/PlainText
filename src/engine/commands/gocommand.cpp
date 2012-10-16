#include "gocommand.h"


#define super Command

GoCommand::GoCommand(QObject *parent) :
    super(parent) {

    setDescription("Go to or enter an exit.\n"
                   "\n"
                   "Examples: go north, go to tower, enter door");
}

GoCommand::~GoCommand() {
}

void GoCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (alias() == "go") {
        takeWord("to", IfNotLast);
    }

    GameObjectPtr exit = takeObject(currentRoom()->exits());
    if (!requireSome(exit, "Go where?")) {
        return;
    }

    player->go(exit);
}
