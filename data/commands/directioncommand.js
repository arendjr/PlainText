function DirectionCommand() {
    Command.call(this);

    this.setDescription("Walk in some direction.\n" +
                        "\n" +
                        "Examples: forward, back, left, right");
}

DirectionCommand.prototype = new Command();
DirectionCommand.prototype.constructor = DirectionCommand;

DirectionCommand.prototype.execute = function(character, command) {

    this.prepareExecute(character, command);

    var direction = this.alias;

    var directionVector = character.direction;
    if (direction === "back") {
        directionVector = [-directionVector[0], -directionVector[1], -directionVector[2]];
    } else if (direction === "left") {
        directionVector = [directionVector[1], -directionVector[0], directionVector[2]];
    } else if (direction === "right") {
        directionVector = [-directionVector[1], directionVector[0], directionVector[2]];
    }

    var suggestions = [];
    for (var i = 0, length = this.currentRoom.portals.length; i < length; i++) {
        var portal = this.currentRoom.portals[i];
        var vector = portal.oppositeOf(this.currentRoom).position.minus(this.currentRoom.position);
        if (vector.angle(directionVector) < Math.PI / 4) {
            suggestions.append(portal);
        }
    }

    if (suggestions.length === 0) {
        character.send("There's no way %1.".arg(direction));
    } else if (suggestions.length === 1) {
        character.go(suggestions[0]);
    } else {
        character.send("There are multiple ways %1, to the %2.".arg(direction,
                                                                    suggestions.joinFancy()));
    }
};

CommandRegistry.registerCommand("forward", new DirectionCommand());
CommandRegistry.registerCommand("back", new DirectionCommand());
CommandRegistry.registerCommand("left", new DirectionCommand());
CommandRegistry.registerCommand("right", new DirectionCommand());
