#include "slashmecommand.h"


SlashMeCommand::SlashMeCommand(Player *character, QObject *parent) :
    Command(character, parent) {
}

SlashMeCommand::~SlashMeCommand() {
}

void SlashMeCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Try being a little bit more creative...")) {
        return;
    }

    QString action = takeRest();

    currentArea()->characters().send(QString("%1 %2").arg(player()->name(), action), Purple);
}
