#include "quitcommand.h"


QuitCommand::QuitCommand(Player *character, QObject *parent) :
    Command(character, parent) {
}

QuitCommand::~QuitCommand() {
}

void QuitCommand::execute(const QString &command) {

    Q_UNUSED(command);

    emit quit();
}
