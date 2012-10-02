#include "quitcommand.h"

#include "util.h"


#define super Command

QuitCommand::QuitCommand(QObject *parent) :
    super(parent) {

    setDescription("Quit the game. Your progress and current location will be saved.\n"
                   "\n"
                   "Examples: quit, goodbye");
}

QuitCommand::~QuitCommand() {
}

void QuitCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    player->quit();
}
