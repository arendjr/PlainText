#include "followcommand.h"

#include "engine/util.h"


FollowCommand::FollowCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Form or join a group by following another player. The first person being "
                   "followed automatically becomes the group leader.\n"
                   "\n"
                   "Example: follow mia");
}

FollowCommand::~FollowCommand() {
}

void FollowCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Follow who?")) {
        return;
    }

    QPair <QString, uint> description = takeObjectsDescription();
    GameObjectPtrList characters = objectsByDescription(description, currentArea()->characters());

    if (characters.isEmpty()) {
        send(QString("\"%1\" is not here.").arg(Util::capitalize(description.first)));
    } else {
        player()->follow(characters[0]);
    }
}
