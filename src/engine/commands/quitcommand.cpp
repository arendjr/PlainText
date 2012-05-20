#include "quitcommand.h"


QuitCommand::QuitCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Quit the game. Your progress and current location will be "
                   "saved.\n"
                   "\n"
                   "Examples: quit, goodbye");
}

QuitCommand::~QuitCommand() {
}

void QuitCommand::execute(const QString &command) {

    Q_UNUSED(command);

    emit quit();
}
