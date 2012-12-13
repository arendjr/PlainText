function OpenCommand() {
    Command.call(this);

    this.setDescription("Open an exit, typically a door or a window. Note that doors " +
                        "automatically close after a while.\n" +
                        "\n" +
                        "Example: open door");
}

OpenCommand.prototype = new Command();
OpenCommand.prototype.constructor = OpenCommand;

OpenCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var portal = this.takeObject(this.currentRoom.portals);
    if (!this.requireSome(portal, "Open what?")) {
        return;
    }

    player.open(portal);
};

CommandRegistry.registerCommand("open", new OpenCommand());
