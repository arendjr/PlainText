#include "addexitcommand.h"

#include "engine/exit.h"
#include "engine/realm.h"
#include "engine/util.h"


AddExitCommand::AddExitCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Add an exit to the current area. Use \"new\" instead of an "
                   "destination area ID to create a new destination area.\n"
                   "\n"
                   "Usage: add-exit <exit-name> <destination-area-id>");
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
            player()->send(QString("No area with ID %1.").arg(destinationAreaId));
            return;
        }
    }

    Exit *exit = GameObject::createByObjectType<Exit *>(realm(), "exit");
    exit->setName(exitName);
    exit->setDestinationArea(destinationArea);
    currentArea()->addExit(exit);

    if (Util::isDirection(exitName)) {
        Exit *oppositeExit = GameObject::createByObjectType<Exit *>(realm(), "exit");
        oppositeExit->setName(Util::opposingDirection(exitName));
        oppositeExit->setDestinationArea(currentArea());
        oppositeExit->setOppositeExit(exit);
        destinationArea->addExit(oppositeExit);
        exit->setOppositeExit(oppositeExit);

        player()->send(QString("Bi-directional exit %1-%2 added.").arg(exitName, oppositeExit->name()));
    } else {
        player()->send(QString("Exit %1 added.").arg(exitName));
    }
}
