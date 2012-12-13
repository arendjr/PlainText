function CloseCommand() {
    Command.call(this);

    this.setDescription("Close an exit, typically a door or a window.\n" +
                        "\n" +
                        "Example: close door");
}

CloseCommand.prototype = new Command();
CloseCommand.prototype.constructor = CloseCommand;

CloseCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var portal = this.takeObject(this.currentRoom.portals);
    if (!this.requireSome(portal, "Close what?")) {
        return;
    }

    player.close(portal);
};

CommandRegistry.registerCommand("close", new CloseCommand());
