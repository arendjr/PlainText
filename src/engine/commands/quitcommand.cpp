#include "quitcommand.h"


QuitCommand::QuitCommand(Character *character, QObject *parent) :
    Command(character, parent) {
}

QuitCommand::~QuitCommand() {
}

void QuitCommand::execute(const QString &command) {

    Q_UNUSED(command);

    emit quit();
}
