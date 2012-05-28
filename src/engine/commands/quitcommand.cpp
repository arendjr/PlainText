#include "quitcommand.h"

#include "engine/util.h"


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

    if (player()->secondsStunned() > 0) {
        player()->send(Util::colorize(QString("Please wait %1 seconds.").arg(player()->secondsStunned()), Olive));
    } else {
        emit quit();
    }
}
