function LookCommand() {
    Command.call(this);

    this.setDescription("Look at something (any object, character, or the current room).\n" +
                        "\n" +
                        "Examples: look, look door, look earl, examine sign");
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
    } else {
        if (!this.assertWordsLeft("Examine what?")) {
            return;
        }
    }

    var description = this.takeObjectsDescription();
    var object = this.objectByDescription(description, player.inventory);
    if (!object) {
        var room = this.currentRoom;
        var pool = room.characters.concat(room.items).concat(room.portals);
        object = this.objectByDescription(description, pool);
    }
    if (!this.requireSome(object, "That's not here.")) {
        return;
    }

    player.send(object.lookAtBy(player));
};

CommandRegistry.registerCommand("examine", new LookCommand());
CommandRegistry.registerCommand("l", new LookCommand());
CommandRegistry.registerCommand("look", new LookCommand());
