#include "exitsetcommand.h"

#include "area.h"
#include "conversionutil.h"
#include "exit.h"
#include "realm.h"
#include "util.h"


#define super ApiCommand

ExitSetCommand::ExitSetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-exit-set <request-id> <exit-id-or-new> <area-from-id> \n"
                   "                     <area-to-id> <name> [<opposite-name>]");
}

ExitSetCommand::~ExitSetCommand() {
}

void ExitSetCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    QString exitId = takeWord();
    QString areaFromId = takeWord();
    QString areaToId = takeWord();
    QString name = takeWord();
    QString oppositeName = takeWord();

    Area *sourceArea = qobject_cast<Area *>(realm()->getObject("area", areaFromId.toUInt()));
    if (sourceArea == nullptr) {
        sendError(404, "Area not found");
        return;
    }

    Exit *exit;
    if (exitId == "new") {
        exit = GameObject::createByObjectType<Exit *>(realm(), "exit");

        sourceArea->addExit(exit);
    } else {
        exit = qobject_cast<Exit *>(realm()->getObject("exit", exitId.toUInt()));
        if (exit == nullptr) {
            sendError(404, "Exit not found");
            return;
        }
    }

    Area *destinationArea = nullptr;
    if (areaToId == "new") {
        destinationArea = GameObject::createByObjectType<Area *>(realm(), "area");
    } else {
        destinationArea = qobject_cast<Area *>(realm()->getObject("area", areaToId.toUInt()));
        if (destinationArea == nullptr) {
            sendError(404, "Area not found");
            return;
        }
    }
    exit->setDestinationArea(destinationArea);

    exit->setName(name);

    Exit *oppositeExit = nullptr;
    if (oppositeName.isEmpty() && Util::isDirection(name)) {
        oppositeName = Util::opposingDirection(name);
    }
    if (!oppositeName.isEmpty()) {
        if (exit->oppositeExit().isNull()) {
            for (const GameObjectPtr &exitPtr : destinationArea->exits()) {
                Exit *destinationAreaExit = exitPtr.cast<Exit *>();
                if (destinationAreaExit->destinationArea() == sourceArea) {
                    oppositeExit = destinationAreaExit;
                }
            }
            if (oppositeExit == nullptr) {
                oppositeExit = GameObject::createByObjectType<Exit *>(realm(), "exit");
                oppositeExit->setDestinationArea(sourceArea);
            }
            oppositeExit->setOppositeExit(exit);
            destinationArea->addExit(oppositeExit);
            exit->setOppositeExit(oppositeExit);
        } else {
            oppositeExit = exit->oppositeExit().cast<Exit *>();
        }
        oppositeExit->setName(oppositeName);
    }

    QVariantMap data;
    data["success"] = true;
    data["exit"] = exit->toJSON(IncludeId | DontIncludeTypeInfo);
    if (destinationArea != nullptr) {
        data["destinationArea"] = destinationArea->toJSON(IncludeId | DontIncludeTypeInfo);;
    }
    if (oppositeExit != nullptr) {
        data["oppositeExit"] = oppositeExit->toJSON(IncludeId | DontIncludeTypeInfo);;
    }
    sendReply(data);
}
