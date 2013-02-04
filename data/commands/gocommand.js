function GoCommand() {
    Command.call(this);

    this.setDescription("Go to or enter an exit.\n" +
                        "\n" +
                        "Examples: go north, go to tower, enter door, go forward");
}

GoCommand.prototype = new Command();
GoCommand.prototype.constructor = GoCommand;

GoCommand.prototype.execute = function(character, command) {

    this.prepareExecute(character, command);

    if (this.alias === "go") {
        var direction = this.peekWord();
        if (direction === "forward" || direction === "back" ||
            direction === "left" || direction === "right") {
            character.execute(direction);
        }

        this.takeWord("to", Options.IfNotLast);
    }

    var portal = this.takeObject(character.currentRoom.portals);
    if (!this.requireSome(portal, "Go where?")) {
        return;
    }

    character.go(portal);
};

CommandRegistry.registerCommand("enter", new GoCommand());
CommandRegistry.registerCommand("go", new GoCommand());
