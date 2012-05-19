#include "saycommand.h"


SayCommand::SayCommand(Player *player, QObject *parent) :
    Command(player, parent) {
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
