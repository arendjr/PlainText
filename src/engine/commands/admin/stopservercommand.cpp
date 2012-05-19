#include "stopservercommand.h"

#include <QCoreApplication>


StopServerCommand::StopServerCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {
}

StopServerCommand::~StopServerCommand() {
}

void StopServerCommand::execute(const QString &command) {

    Q_UNUSED(command);

    qApp->quit();
}
