#include "losecommand.h"

#include "util.h"


#define super Command

LoseCommand::LoseCommand(QObject *parent) :
    super(parent) {

    setDescription("Remove yourself or someone else from a group. If you are a group leader, you "
                   "can remove anyone from your group by using *lose <name>*. You can always "
                   "remove yourself from a group using simply *lose*.\n"
                   "\n"
                   "Examples: lose mia, lose");
}

LoseCommand::~LoseCommand() {
}

void LoseCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (hasWordsLeft()) {
        if (peekWord() == "all") {
            player->disband();
            return;
        }

        GameObjectPtr character = takeObject(currentRoom()->characters());
        if (!requireSome(character, "Lose who?")) {
            return;
        }

        player->lose(character);
    } else {
        player->lose();
    }
}
