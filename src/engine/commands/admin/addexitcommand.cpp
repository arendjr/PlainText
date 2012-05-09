#include "addexitcommand.h"

#include "engine/exit.h"
#include "engine/realm.h"
#include "engine/util.h"


AddExitCommand::AddExitCommand(Character *character, QObject *parent) :
    AdminCommand(character, parent) {
}

AddExitCommand::~AddExitCommand() {
}

void AddExitCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: add-exit <exit-name> <destination-area-id>")) {
        return;
    }

    QString exitName = takeWord();
    QString destinationAreaId = takeWord();

    Area *destinationArea;
    if (destinationAreaId == "new") {
        destinationArea = qobject_cast<Area *>(GameObject::createByObjectType("area"));
    } else {
        destinationArea = qobject_cast<Area *>(Realm::instance()->getObject("area", destinationAreaId.toInt()));
        if (!destinationArea) {
            character()->send(QString("No area with ID %1.").arg(destinationAreaId));
            return;
        }
    }

    Exit *exit = qobject_cast<Exit *>(GameObject::createByObjectType("exit"));
    exit->setName(exitName);
    exit->setDestinationArea(destinationArea);
    currentArea()->addExit(exit);

    if (Util::isDirection(exitName)) {
        exit = qobject_cast<Exit *>(GameObject::createByObjectType("exit"));
        exit->setName(Util::opposingDirection(exitName));
        exit->setDestinationArea(currentArea());
        destinationArea->addExit(exit);

        character()->send(QString("Bi-directional exit %1-%2 added.").arg(exitName, exit->name()));
    } else {
        character()->send(QString("Exit %1 added.").arg(exitName));
    }
}
