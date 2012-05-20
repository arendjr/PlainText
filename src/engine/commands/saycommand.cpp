#include "saycommand.h"


SayCommand::SayCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Say a message. Everyone in the same area can hear it.\n"
                   "\n"
                   "Example: say How are you?");
}

SayCommand::~SayCommand() {
}

void SayCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Say what?")) {
        return;
    }

    QString message = takeRest();

    player()->say(message);
}
