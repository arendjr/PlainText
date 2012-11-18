#include "addportalcommand.h"

#include "point3d.h"
#include "portal.h"
#include "realm.h"
#include "util.h"
#include "vector3d.h"


#define super AdminCommand

AddPortalCommand::AddPortalCommand(QObject *parent) :
    super(parent) {

    setDescription("Add a portal to the current room.\n"
                   "\n"
                   "Usage: add-portal <portal-name> <destination-id> [<distance-or-vector>]\n"
                   "\n"
                   "Use \"new\" instead of a destination ID to create a new destination room.\n"
                   "\n"
                   "You can specify two portal names, separated by a dash, to give the portal "
                   "different names depending on the room from which it is referred. If the portal "
                   "name is a direction (north, up, etc.), the opposite direction is chosen "
                   "automatically as the portal name from the destination room.\n"
                   "\n"
                   "The optional distance-or-vector parameter is only allowed when a new "
                   "destination room is created and serves to give it a position in 3D space, "
                   "relative to the current room. The parameter specifies a single number, or a "
                   "vector, each defined in meters. For example, if the portal name is north and "
                   "the give distance is 10, the new room will be 10 meters north of the current "
                   "room.");
}

AddPortalCommand::~AddPortalCommand() {
}

void AddPortalCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    QString portalName = takeWord();
    if (!assertWordsLeft("Usage: add-portal <exit-name> <destination-id> [<distance-or-vector>]")) {
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

    QString oppositeName;
    if (Util::isDirection(portalName)) {
        oppositeName = Util::opposingDirection(portalName);
    } else if (portalName.contains('-')) {
        oppositeName = portalName.section('-', 1);
        portalName = portalName.section('-', 0, 0);
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
            if (!Util::isDirection(portalName)) {
                send("Giving a distance is only supported when the portal name is a direction.");
                return;
            }
            position = currentRoom()->position() + distance * Util::vectorForDirection(portalName);
        }
    }

    if (!destination) {
        destination = new Room(realm());
        destination->setPosition(position);
    }

    Portal *portal = new Portal(realm());
    portal->setName(portalName);
    portal->setRoom(currentRoom());
    portal->setName2(oppositeName);
    portal->setRoom2(destination);

    currentRoom()->addPortal(portal);
    destination->addPortal(portal);

    send("Portal %1 added.", portalName);
}
