#include "exitsetcommand.h"

#include "room.h"
#include "conversionutil.h"
#include "exit.h"
#include "point3d.h"
#include "realm.h"
#include "util.h"


#define super ApiCommand

ExitSetCommand::ExitSetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-exit-set <request-id> <exit-id-or-new> <room-from-id> \n"
                   "                     <room-to-id> <name> [<opposite-name>] [<position>]");
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
    QString position = takeRest();

    Room *source = qobject_cast<Room *>(realm()->getObject(GameObjectType::Room,
                                                           roomFromId.toUInt()));
    if (source == nullptr) {
        sendError(404, "Room not found");
        return;
    }

    Exit *exit = nullptr;
    if (exitId != "new") {
        exit = qobject_cast<Exit *>(realm()->getObject(GameObjectType::Exit, exitId.toUInt()));
        if (exit == nullptr) {
            sendError(404, "Exit not found");
            return;
        }
    }

    Room *destination = nullptr;
    if (roomToId != "new") {
        destination = qobject_cast<Room *>(realm()->getObject(GameObjectType::Room,
                                                              roomToId.toUInt()));
        if (destination == nullptr) {
            sendError(404, "Room not found");
            return;
        }
    }

    Point3D point(0, 0, 0);
    if (!position.isEmpty()) {
        point = Point3D::fromUserString(position);
    }

    if (!exit) {
        exit = new Exit(realm());
        source->addExit(exit);
    }

    if (!destination) {
        destination = new Room(realm());
        destination->setPosition(point);
    }

    Exit *oppositeExit = nullptr;
    if (oppositeName.isEmpty() && Util::isDirection(name)) {
        oppositeName = Util::opposingDirection(name);
    }
    if (!oppositeName.isEmpty() && !oppositeName.startsWith("(")) {
        if (exit->oppositeExit().isNull()) {
            for (const GameObjectPtr &exitPtr : destination->exits()) {
                Exit *destinationExit = exitPtr.cast<Exit *>();
                if (destinationExit->destination() == source) {
                    oppositeExit = destinationExit;
                }
            }
        } else {
            oppositeExit = exit->oppositeExit().cast<Exit *>();
        }
    }
    if (!oppositeExit) {
        oppositeExit = new Exit(realm());
        oppositeExit->setDestination(source);
        destination->addExit(oppositeExit);
    }

    exit->setName(name);
    exit->setDestination(destination);
    exit->setOppositeExit(oppositeExit);

    oppositeExit->setName(oppositeName);
    oppositeExit->setDestination(source);
    oppositeExit->setOppositeExit(exit);

    QVariantMap data;
    data["success"] = true;
    data["exit"] = exit->toJsonString();
    data["source"] = source->toJsonString();
    if (destination != nullptr) {
        data["destination"] = destination->toJsonString();
    }
    if (oppositeExit != nullptr) {
        data["oppositeExit"] = oppositeExit->toJsonString();
    }
    sendReply(data);
}
