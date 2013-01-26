#include "portalsetcommand.h"

#include "area.h"
#include "conversionutil.h"
#include "point3d.h"
#include "portal.h"
#include "realm.h"
#include "room.h"
#include "util.h"


#define super ApiCommand

PortalSetCommand::PortalSetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-portal-set <request-id> <portal-id-or-new> <room-from-id> \n"
                   "                       <room-to-id> <name> <opposite-name> <flags> \n"
                   "                       [<position>]");
}

PortalSetCommand::~PortalSetCommand() {
}

void PortalSetCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    QString portalId = takeWord();
    QString roomFromId = takeWord();
    QString roomToId = takeWord();
    QString name = takeWord();
    QString oppositeName = takeWord();
    QString flags = takeWord();
    QString position = takeRest();

    Room *source = qobject_cast<Room *>(realm()->getObject(GameObjectType::Room,
                                                           roomFromId.toUInt()));
    if (source == nullptr) {
        sendError(404, "Room not found");
        return;
    }

    Portal *portal = nullptr;
    if (portalId != "new") {
        portal = qobject_cast<Portal *>(realm()->getObject(GameObjectType::Portal,
                                                           portalId.toUInt()));
        if (portal == nullptr) {
            sendError(404, "Portal not found");
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
        Point3D::fromUserString(position, point);
    }

    if (!destination) {
        destination = new Room(realm());
        destination->setPosition(point);

        if (!source->area().isNull()) {
            source->area().cast<Area *>()->addRoom(destination);
        }
    }

    if (!portal) {
        portal = new Portal(realm());
    }

    source->addPortal(portal);
    destination->addPortal(portal);

    if (oppositeName.isEmpty() && Util::isDirection(name)) {
        oppositeName = Util::opposingDirection(name);
    }

    portal->setName(name);
    portal->setRoom(source);

    portal->setName2(oppositeName);
    portal->setRoom2(destination);

    portal->setFlags(PortalFlags::fromString(flags));

    QVariantMap data;
    data["success"] = true;
    data["portal"] = portal->toJsonString();
    data["source"] = source->toJsonString();
    data["destination"] = destination->toJsonString();
    if (!source->area().isNull()) {
        data["area"] = source->area()->toJsonString();
    }
    sendReply(data);
}
