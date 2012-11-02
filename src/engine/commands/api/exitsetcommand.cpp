#include "exitsetcommand.h"

#include "room.h"
#include "conversionutil.h"
#include "exit.h"
#include "realm.h"
#include "util.h"


#define super ApiCommand

ExitSetCommand::ExitSetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-exit-set <request-id> <exit-id-or-new> <room-from-id> \n"
                   "                     <room-to-id> <name> [<opposite-name>]");
}

ExitSetCommand::~ExitSetCommand() {
}

void ExitSetCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    QString exitId = takeWord();
    QString roomFromId = takeWord();
    QString roomToId = takeWord();
    QString name = takeWord();
    QString oppositeName = takeWord();

    Room *source = qobject_cast<Room *>(realm()->getObject("area", roomFromId.toUInt()));
    if (source == nullptr) {
        sendError(404, "Area not found");
        return;
    }

    Exit *exit;
    if (exitId == "new") {
        exit = new Exit(realm());

        source->addExit(exit);
    } else {
        exit = qobject_cast<Exit *>(realm()->getObject("exit", exitId.toUInt()));
        if (exit == nullptr) {
            sendError(404, "Exit not found");
            return;
        }
    }

    Room *destination;
    if (roomToId == "new") {
        destination = new Room(realm());
    } else {
        destination = qobject_cast<Room *>(realm()->getObject("room", roomToId.toUInt()));
        if (destination == nullptr) {
            sendError(404, "Room not found");
            return;
        }
    }
    exit->setDestination(destination);

    exit->setName(name);

    Exit *oppositeExit = nullptr;
    if (oppositeName.isEmpty() && Util::isDirection(name)) {
        oppositeName = Util::opposingDirection(name);
    }
    if (!oppositeName.isEmpty()) {
        if (exit->oppositeExit().isNull()) {
            for (const GameObjectPtr &exitPtr : destination->exits()) {
                Exit *destinationExit = exitPtr.cast<Exit *>();
                if (destinationExit->destination() == source) {
                    oppositeExit = destinationExit;
                }
            }
            if (oppositeExit == nullptr) {
                oppositeExit = new Exit(realm());
                oppositeExit->setDestination(source);
            }
            oppositeExit->setOppositeExit(exit);
            destination->addExit(oppositeExit);
            exit->setOppositeExit(oppositeExit);
        } else {
            oppositeExit = exit->oppositeExit().cast<Exit *>();
        }
        oppositeExit->setName(oppositeName);
    }

    QVariantMap data;
    data["success"] = true;
    data["exit"] = exit->toJsonString();
    if (destination != nullptr) {
        data["destination"] = destination->toJsonString();
    }
    if (oppositeExit != nullptr) {
        data["oppositeExit"] = oppositeExit->toJsonString();
    }
    sendReply(data);
}
