#include "saycommand.h"


#define super Command

SayCommand::SayCommand(QObject *parent) :
    super(parent) {

    setDescription("Say a message. Everyone in the same area can hear it.\n"
                   "\n"
                   "Example: say How are you?");
}

SayCommand::~SayCommand() {
}

void SayCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    if (!assertWordsLeft("Say what?")) {
        return;
    }

    QString message = takeRest();

    player->say(message);
}
