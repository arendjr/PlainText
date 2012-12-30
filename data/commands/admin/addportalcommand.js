function AddPortalCommand() {
    AdminCommand.call(this);

    this.setDescription("Add a portal to the current room.\n" +
                        "\n" +
                        "Usage: add-portal <portal-name> <destination-id> " +
                        "[<distance-or-vector>]\n" +
                        "\n" +
                        "Destination ID is the ID of a room to which the portal should link. Use " +
                        "\"new\" instead of a destination ID to create a new destination room.\n" +
                        "\n" +
                        "You can specify two portal names, separated by a dash, to give the " +
                        "portal a different name from the destination room. If the portal name " +
                        "is a direction (north, up, etc.), the opposite direction is chosen " +
                        "automatically as the portal name from the destination room.\n" +
                        "\n" +
                        "The optional distance-or-vector parameter is only allowed when a new " +
                        "destination room is created and serves to give it a position in 3D " +
                        "space, relative to the current room. The parameter specifies a single " +
                        "number, or a vector, each defined in meters. For example, if the portal " +
                        "name is north and the give distance is 10, the new room will be 10 " +
                        "meters north of the current room.");
}

AddPortalCommand.prototype = new AdminCommand();
AddPortalCommand.prototype.constructor = AddPortalCommand;

AddPortalCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var portalName = this.takeWord();
    if (!this.assertWordsLeft("Usage: add-portal <portal-name> <destination-id> " +
                              "[<distance-or-vector>]")) {
        return;
    }

    var destinationId = this.takeWord();

    var destination = null;
    if (destinationId !== "new") {
        destination = Realm.getObject("Room", destinationId.toInt());
        if (!destination) {
            this.send("No room with ID %1.", destinationId);
            return;
        }
    }

    var oppositeName;
    if (Util.isDirection(portalName)) {
        oppositeName = Util.opposingDirection(portalName);
    } else if (portalName.contains("-")) {
        oppositeName = portalName.section("-", 1);
        portalName = portalName.section("-", 0, 0);
    }

    var position = [0, 0, 0];
    if (destinationId === "new") {
        var distanceOrVector = this.takeRest();
        if (distanceOrVector.startsWith("[") && distanceOrVector.endsWith("]")) {
            var components = Util.splitComponents(distanceOrVector);
            var vector = [components[0].toInt(), components[1].toInt(), components[2].toInt()];
            position = this.currentRoom.position.plus(vector);
        } else if (!distanceOrVector.isEmpty()) {
            var distance = distanceOrVector.toInt();
            if (distance === 0) {
                this.send("Invalid distance or vector given.");
                return;
            }
            if (!Util.isDirection(portalName)) {
                this.send("Giving a distance is only supported when the portal name is a " +
                          "direction.");
                return;
            }
            position = this.currentRoom.position.plus(Util.vectorForDirection(portalName)
                                                      .mul(distance));
        }
    }

    if (!destination) {
        destination = Realm.createObject("Room");
        destination.position = position;

        if (this.currentRoom.area) {
            this.currentRoom.area.addRoom(destination);
        }
    }

    var portal = Realm.createObject("Portal");
    portal.name = portalName;
    portal.room = this.currentRoom;
    portal.name2 = oppositeName;
    portal.room2 = destination;

    this.currentRoom.addPortal(portal);
    destination.addPortal(portal);

    this.send("Portal %1 added.", portalName);
};

CommandRegistry.registerAdminCommand("add-portal", new AddPortalCommand());
