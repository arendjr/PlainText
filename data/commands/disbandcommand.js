function DisbandCommand() {
    Command.call(this);

    this.setDescription("Disband your group. Only the group leader can disband a group.\n" +
                        "\n" +
                        "Example: disband");
}

DisbandCommand.prototype = new Command();
DisbandCommand.prototype.constructor = DisbandCommand;

DisbandCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    player.disband();
}

CommandRegistry.registerCommand("disband", new DisbandCommand());
