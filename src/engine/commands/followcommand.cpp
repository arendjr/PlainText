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

    if (!assertWordsLeft("Follow who?")) {
        return;
    }

    QPair <QString, uint> description = takeObjectsDescription();
    GameObjectPtrList characters = objectsByDescription(description, currentArea()->characters());

    if (characters.isEmpty()) {
        send(QString("\"%1\" is not here.").arg(Util::capitalize(description.first)));
    } else {
        player->follow(characters[0]);
    }
}
