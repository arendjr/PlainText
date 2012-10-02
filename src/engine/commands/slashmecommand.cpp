#include "slashmecommand.h"


#define super Command

SlashMeCommand::SlashMeCommand(QObject *parent) :
    super(parent) {
}

SlashMeCommand::~SlashMeCommand() {
}

void SlashMeCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    if (!assertWordsLeft("Try being a little bit more creative...")) {
        return;
    }

    QString action = takeRest();

    currentArea()->characters().send(QString("%1 %2").arg(player->name(), action), Purple);
}
