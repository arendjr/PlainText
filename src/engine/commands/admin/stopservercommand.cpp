#include "stopservercommand.h"

#include <QCoreApplication>


#define super AdminCommand

StopServerCommand::StopServerCommand(QObject *parent) :
    super(parent) {

    setDescription("Terminate the game server.\n"
                   "\n"
                   "Usage: stop-server");
}

StopServerCommand::~StopServerCommand() {
}

void StopServerCommand::execute(Character *player, const QString &command) {

    Q_UNUSED(player);
    Q_UNUSED(command);

    qApp->quit();
}
