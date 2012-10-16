#include "addcontainercommand.h"

#include "container.h"


#define super AdminCommand

AddContainerCommand::AddContainerCommand(QObject *parent) :
    super(parent) {

    setDescription("Add a container to the current area.\n"
                   "\n"
                   "Usage: add-container <container-name>");
}

AddContainerCommand::~AddContainerCommand() {
}

void AddContainerCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: add-container <container-name>")) {
        return;
    }

    QString containerName = takeWord();

    Container *container = GameObject::createByObjectType<Container *>(realm(), "container");
    container->setName(containerName);
    container->setPortable(true);
    currentRoom()->addItem(container);

    send(QString("Container %1 added.").arg(containerName));
}
