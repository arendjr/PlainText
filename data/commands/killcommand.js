function KillCommand() {
    Command.call(this);

    this.setDescription("Attack another character.\n" +
                        "\n" +
                        "Examples: kill joe, attack earl");
}

KillCommand.prototype = new Command();
KillCommand.prototype.constructor = KillCommand;

KillCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var character = this.takeObject(this.currentRoom.characters);
    if (!this.requireSome(character, "Kill who?")) {
        return;
    }

    player.kill(character);
};

CommandRegistry.registerCommand("attack", new KillCommand());
CommandRegistry.registerCommand("kill", new KillCommand());
