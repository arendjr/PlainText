#include "saycommand.h"

#include "engine/util.h"


SayCommand::SayCommand(Character *character, QObject *parent) :
    Command(character, parent) {
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

    Util::sendOthers(currentArea()->characters(),
                     QString("%1 says, \"%2\".").arg(character()->name(), message));
}
