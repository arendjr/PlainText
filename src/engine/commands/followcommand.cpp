#include "followcommand.h"

#include "util.h"


#define super Command

FollowCommand::FollowCommand(QObject *parent) :
    super(parent) {

    setDescription("Form or join a group by following another player. The first person being "
                   "followed automatically becomes the group leader.\n"
                   "\n"
                   "Example: follow mia");
}

FollowCommand::~FollowCommand() {
}

void FollowCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr character = takeObject(currentRoom()->characters());
    if (!requireSome(character, "Follow who?")) {
        return;
    }

    player->follow(character);
}
