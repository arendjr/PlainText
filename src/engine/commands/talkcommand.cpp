#include "talkcommand.h"

#include "util.h"


#define super Command

TalkCommand::TalkCommand(QObject *parent) :
    super(parent) {

    setDescription("Talk to a specific character who's in the same area. Only "
                   "this character will hear you.\n"
                   "\n"
                   "Example: talk earl Hey Earl, how are you?");
}

TalkCommand::~TalkCommand() {
}

void TalkCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    takeWord("to");

    GameObjectPtr character = takeObject(currentRoom()->characters());
    if (!requireSome(character, "Talk to who?")) {
        return;
    }

    if (!assertWordsLeft("Say what?")) {
        return;
    }

    QString message = takeRest();

    player->talk(character, message);
}
