#include "shoutcommand.h"


ShoutCommand::ShoutCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Shout a message. Everyone in the same area and every "
                   "adjacent area can hear it.\n"
                   "\n"
                   "Example: shout Anyone there?");
}

ShoutCommand::~ShoutCommand() {
}

void ShoutCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Shout what?")) {
        return;
    }

    QString message = takeRest();

    player()->shout(message);
}
