#include "addexitcommand.h"

#include "engine/exit.h"
#include "engine/realm.h"
#include "engine/util.h"


AddExitCommand::AddExitCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Add an exit to the current area.\n"
                   "\n"
                   "Usage: add-exit <exit-name> <destination-area-id>\n"
                   "\n"
                   "Use \"new\" instead of a destination area ID to create a new destination "
                   "area.\n"
                   "\n"
                   "You can create a bi-directional exit (the destination area will get an exit "
                   "back to the current area) by using a direction (north, up, etc.) as exit name "
                   "or by specifying two exit names, separated by a dash, instead of one.\n");
}

AddExitCommand::~AddExitCommand() {
}

void AddExitCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    QString exitName = takeWord();
    if (!assertWordsLeft("Usage: add-exit <exit-name> <destination-area-id>")) {
        return;
    }

    QString destinationAreaId = takeWord();

    Area *destinationArea;
    if (destinationAreaId == "new") {
        destinationArea = GameObject::createByObjectType<Area *>(realm(), "area");
    } else {
        destinationArea = qobject_cast<Area *>(realm()->getObject("area",
                                                                  destinationAreaId.toInt()));
        if (!destinationArea) {
            send(QString("No area with ID %1.").arg(destinationAreaId));
            return;
        }
    }

    QString oppositeExitName;
    if (Util::isDirection(exitName)) {
        oppositeExitName = Util::opposingDirection(exitName);
    } else if (exitName.contains('-')) {
        oppositeExitName = exitName.section('-', 1);
        exitName = exitName.section('-', 0, 0);
    } else if (destinationAreaId == "new") {
        send("When adding an exit to a new area, the exit has to be bi-directional.");
        return;
    }

    Exit *exit = GameObject::createByObjectType<Exit *>(realm(), "exit");
    exit->setName(exitName);
    exit->setDestinationArea(destinationArea);
    currentArea()->addExit(exit);

    send(QString("Exit %1 added.").arg(exitName));

    if (!oppositeExitName.isEmpty()) {
        Exit *oppositeExit = GameObject::createByObjectType<Exit *>(realm(), "exit");
        oppositeExit->setName(oppositeExitName);
        oppositeExit->setDestinationArea(currentArea());
        oppositeExit->setOppositeExit(exit);
        destinationArea->addExit(oppositeExit);
        exit->setOppositeExit(oppositeExit);

        send(QString("Opposite exit %1 added.").arg(oppositeExit->name()));
    }
}
