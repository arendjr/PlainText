#include "addcontainercommand.h"

#include "container.h"


#define super AdminCommand

AddContainerCommand::AddContainerCommand(QObject *parent) :
    super(parent) {

    setDescription("Add a container to the current room.\n"
                   "\n"
                   "Usage: add-container <container-name>");
}

AddContainerCommand::~AddContainerCommand() {
}

void AddContainerCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: add-container <container-name>")) {
        return;
    }

    QString containerName = takeWord();

    Container *container = new Container(realm());
    container->setName(containerName);
    container->setPortable(true);
    currentRoom()->addItem(container);

    send("Container %1 added.", containerName);
}
