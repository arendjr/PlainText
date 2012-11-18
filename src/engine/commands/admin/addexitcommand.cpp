#include "addexitcommand.h"

#include "exit.h"
#include "point3d.h"
#include "realm.h"
#include "util.h"
#include "vector3d.h"


#define super AdminCommand

AddExitCommand::AddExitCommand(QObject *parent) :
    super(parent) {

    setDescription("Add an exit to the current room.\n"
                   "\n"
                   "Usage: add-exit <exit-name> <destination-id> [<distance-or-vector>]\n"
                   "\n"
                   "Use \"new\" instead of a destination ID to create a new destination room.\n"
                   "\n"
                   "You can create a bi-directional exit (the destination room will get an exit "
                   "back to the current room) by using a direction (north, up, etc.) as exit name "
                   "or by specifying two exit names, separated by a dash, instead of one.\n"
                   "\n"
                   "The optional distance-or-vector parameter is only allowed when a new "
                   "destination room is created and serves to give it a position in 3D space, "
                   "relative to the current room. The parameter specifies a single number, or a "
                   "vector, each defined in meters. For example, if the exit name is north, and "
                   "the give distance is 10, the new room will be 10 meters north of the current "
                   "room.");
}

AddExitCommand::~AddExitCommand() {
}

void AddExitCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    QString exitName = takeWord();
    if (!assertWordsLeft("Usage: add-exit <exit-name> <destination-id> [<distance-or-vector>]")) {
        return;
    }

    QString destinationId = takeWord();

    Room *destination = nullptr;
    if (destinationId != "new") {
        destination = qobject_cast<Room *>(realm()->getObject(GameObjectType::Room,
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

    Point3D position(0, 0, 0);
    if (destinationId == "new") {
        QString distanceOrVector = takeRest();
        if (distanceOrVector.startsWith("[") && distanceOrVector.endsWith("]")) {
            Vector3D vector;
            Vector3D::fromUserString(distanceOrVector, vector);
            position = currentRoom()->position() + vector;
        } else if (!distanceOrVector.isEmpty()) {
            int distance = distanceOrVector.toInt();
            if (distance == 0) {
                send("Invalid distance or vector given.");
                return;
            }
            if (!Util::isDirection(exitName)) {
                send("Giving a distance is only supported when the exit name is a direction.");
                return;
            }
            position = currentRoom()->position() + distance * Util::vectorForDirection(exitName);
        }
    }

    if (!destination) {
        destination = new Room(realm());
        destination->setPosition(position);
    }

    Exit *exit = new Exit(realm());
    exit->setName(exitName);
    exit->setDestination(destination);
    currentRoom()->addExit(exit);

    send("Exit %1 added.", exitName);

    if (!oppositeExitName.isEmpty()) {
        Exit *oppositeExit = new Exit(realm());
        oppositeExit->setName(oppositeExitName);
        oppositeExit->setDestination(currentRoom());
        oppositeExit->setOppositeExit(exit);
        destination->addExit(oppositeExit);
        exit->setOppositeExit(oppositeExit);

        send("Opposite exit %1 added.", oppositeExit->name());
    }
}
