#include "saycommand.h"


#define super Command

SayCommand::SayCommand(QObject *parent) :
    super(parent) {

    setDescription("Say something. Everyone in the same room can hear it.\n"
                   "\n"
                   "Example: say How are you?");
}

SayCommand::~SayCommand() {
}

void SayCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Say what?")) {
        return;
    }

    QString message = takeRest();

    player->say(message);
}
