function LookCommand() {
    Command.call(this);

    this.setDescription("Look at something (any object, character, or the current room).\n" +
                        "\n" +
                        "Examples: look, look at door, look earl, examine sign,\n" +
                        "          look at key in inventory");
}

LookCommand.prototype = new Command();
LookCommand.prototype.constructor = LookCommand;

LookCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (this.alias === "l" || this.alias === "look") {
        if (!this.hasWordsLeft()) {
            player.send(player.currentRoom.lookAtBy(player));
            return;
        }

        this.takeWord("at", Options.IfNotLast);
        if (!this.assertWordsLeft("Look at what?")) {
            return;
        }

        if (this.peekWord() === "inventory" || this.peekRest() === "in inventory") {
            player.execute("inventory");
            return;
        }
    } else {
        if (!this.assertWordsLeft("Examine what?")) {
            return;
        }
    }

    var room = this.currentRoom;
    var description = this.takeObjectsDescription();
    var pool;
    if (this.peekRest() === "in inventory") {
        pool = player.inventory;
    } else {
        pool = room.characters.concat(room.items).concat(room.portals).concat(player.inventory);
    }
    var object = this.objectByDescription(description, pool);
    if (!this.requireSome(object, "That's not here.")) {
        return;
    }

    player.send(object.lookAtBy(player));

    function describeNearbyObject(nearbyObject, vector, position) {
        if (vector.angle(position) < Math.PI / 8) {
            var angle = Util.angleBetweenDirectionAndPosition(vector, position);

            if (nearbyObject.isPortal()) {
                player.send("On its %1 is %2.".arg(angle > 0 ? "right" : "left",
                                                   nearbyObject.nameWithDestinationFromRoom(room)));
            } else {
                player.send("On its %1 there's %2.".arg(angle > 0 ? "right" : "left",
                                                        nearbyObject.indefiniteName()));
            }
        }
    }

    var showNearbyObjects = (object.isItem() && !object.position.equals([0, 0, 0]) ||
                             object.isPortal());
    if (showNearbyObjects) {
        var vector = (object.isPortal() ? object.position.minus(room.position) : object.position);
        room.items.forEach(function(item) {
            if (item === object) {
                return;
            }

            var position = item.position;
            describeNearbyObject(item, vector, position);
        });
        room.portals.forEach(function(portal) {
            if (portal === object) {
                return;
            }

            var position = portal.position.minus(room.position);
            describeNearbyObject(portal, vector, position);
        });
    }

    if (object.isPortal() && object.canSeeThrough()) {
        var characters = VisualUtil.charactersVisibleThroughPortal(player.currentRoom, object);
        var characterText = VisualUtil.describeCharactersRelativeTo(characters, player);
        if (characterText !== "") {
            player.send(characterText);
        }
    }
};

CommandRegistry.registerCommand("examine", new LookCommand());
CommandRegistry.registerCommand("l", new LookCommand());
CommandRegistry.registerCommand("look", new LookCommand());
