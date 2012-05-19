#include "shoutcommand.h"


ShoutCommand::ShoutCommand(Player *character, QObject *parent) :
    Command(character, parent) {
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
