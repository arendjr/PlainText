#include "disbandcommand.h"


#define super Command

DisbandCommand::DisbandCommand(QObject *parent) :
    super(parent) {

    setDescription("Disband your group. Only the group leader can disband a group.\n"
                   "\n"
                   "Example: disband");
}

DisbandCommand::~DisbandCommand() {
}

void DisbandCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    player->disband();
}
