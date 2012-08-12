#include "disbandcommand.h"

#include "engine/util.h"


DisbandCommand::DisbandCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Disband your group. Only the group leader can disband a group.\n"
                   "\n"
                   "Example: disband");
}

DisbandCommand::~DisbandCommand() {
}

void DisbandCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    player()->disband();
}
