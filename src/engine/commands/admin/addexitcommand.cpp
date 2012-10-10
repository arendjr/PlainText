#include "addexitcommand.h"

#include "exit.h"
#include "realm.h"
#include "util.h"


#define super AdminCommand

AddExitCommand::AddExitCommand(QObject *parent) :
    super(parent) {

    setDescription("Add an exit to the current area.\n"
                   "\n"
                   "Usage: add-exit <exit-name> <destination-id>\n"
                   "\n"
                   "Use \"new\" instead of a destination ID to create a new destination room.\n"
                   "\n"
                   "You can create a bi-directional exit (the destination room will get an exit "
                   "back to the current room) by using a direction (north, up, etc.) as exit name "
                   "or by specifying two exit names, separated by a dash, instead of one.\n");
}

AddExitCommand::~AddExitCommand() {
}

void AddExitCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    QString exitName = takeWord();
    if (!assertWordsLeft("Usage: add-exit <exit-name> <destination-id>")) {
        return;
    }

    QString destinationId = takeWord();

    Room *destination;
    if (destinationId == "new") {
        destination = GameObject::createByObjectType<Room *>(realm(), "room");
    } else {
        destination = qobject_cast<Room *>(realm()->getObject("room",
                                                                  destinationId.toInt()));
        if (!destination) {
            send(QString("No room with ID %1.").arg(destinationId));
            return;
        }
    }

    QString oppositeExitName;
    if (Util::isDirection(exitName)) {
        oppositeExitName = Util::opposingDirection(exitName);
    } else if (exitName.contains('-')) {
        oppositeExitName = exitName.section('-', 1);
        exitName = exitName.section('-', 0, 0);
    } else if (destinationId == "new") {
        send("When adding an exit to a new room, the exit has to be bi-directional.");
        return;
    }

    Exit *exit = GameObject::createByObjectType<Exit *>(realm(), "exit");
    exit->setName(exitName);
    exit->setDestination(destination);
    currentRoom()->addExit(exit);

    send(QString("Exit %1 added.").arg(exitName));

    if (!oppositeExitName.isEmpty()) {
        Exit *oppositeExit = GameObject::createByObjectType<Exit *>(realm(), "exit");
        oppositeExit->setName(oppositeExitName);
        oppositeExit->setDestination(currentRoom());
        oppositeExit->setOppositeExit(exit);
        destination->addExit(oppositeExit);
        exit->setOppositeExit(oppositeExit);

        send(QString("Opposite exit %1 added.").arg(oppositeExit->name()));
    }
}
