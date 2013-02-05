#include "portalsetcommand.h"

#include "qjson/json_driver.hh"

#include "area.h"
#include "conversionutil.h"
#include "gameexception.h"
#include "point3d.h"
#include "portal.h"
#include "realm.h"
#include "room.h"
#include "util.h"


#define super ApiCommand

PortalSetCommand::PortalSetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-portal-set <request-id> <portal-JSON>");
}

PortalSetCommand::~PortalSetCommand() {
}

void PortalSetCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    bool error;
    JSonDriver driver;
    QString jsonString = takeRest();
    QVariantMap map = driver.parse(jsonString, &error).toMap();
    if (error) {
        throw GameException(GameException::InvalidGameObjectJson, jsonString);
    }

    uint roomId = map["room"].toUInt();
    Room *room = qobject_cast<Room *>(realm()->getObject(GameObjectType::Room, roomId));
    if (room == nullptr) {
        sendError(404, "Room not found");
        return;
    }

    uint room2Id = 0;
    Room *room2 = nullptr;
    if (map["room2"].toString() != "new") {
        room2Id = map["room2"].toUInt();
        room2 = qobject_cast<Room *>(realm()->getObject(GameObjectType::Room, room2Id));
        if (room2 == nullptr) {
            sendError(404, "Room not found");
            return;
        }
    }

    uint portalId = 0;
    Portal *portal = nullptr;
    if (map["id"].toString() != "new") {
        portalId = map["id"].toUInt();
        portal = qobject_cast<Portal *>(realm()->getObject(GameObjectType::Portal, portalId));
        if (portal == nullptr) {
            sendError(404, "Portal not found");
            return;
        }
    }

    if (!room2) {
        room2 = new Room(realm());

        Point3D position;
        Point3D::fromVariant(map["position"], position);
        room2->setPosition(position);

        if (!room->area().isNull()) {
            room->area().cast<Area *>()->addRoom(room2);
        }
    }

    if (!portal) {
        portal = new Portal(realm());
    }

    room->addPortal(portal);
    room2->addPortal(portal);

    portal->setRoom(room);
    portal->setName(map["name"].toString());
    portal->setDestination(map["destination"].toString());
    portal->setDescription(map["description"].toString());

    portal->setRoom2(room2);
    portal->setName2(map["name2"].toString());
    portal->setDestination2(map["destination2"].toString());
    portal->setDescription2(map["description2"].toString());

    portal->setFlags(PortalFlags::fromString(map["flags"].toString()));

    GameEventMultiplierMap eventMultipliers;
    GameEventMultiplierMap::fromVariant(map["eventMultipliers"], eventMultipliers);
    portal->setEventMultipliers(eventMultipliers);

    QVariantMap data;
    data["success"] = true;
    data["portal"] = portal->toJsonString();
    data["room"] = room->toJsonString();
    data["room2"] = room2->toJsonString();
    if (!room->area().isNull()) {
        data["area"] = room->area()->toJsonString();
    }
    sendReply(data);
}
