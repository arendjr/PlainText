#include "saycommand.h"


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

    foreach (const GameObjectPtr &other, currentArea()->characters()) {
        other.cast<Character *>()->send(QString("%1 says, \"%2\".").arg(character()->name(), message));
    }
}
