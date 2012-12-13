function WhoCommand() {
    Command.call(this);

    this.setDescription("Show who is currently online.\n" +
                        "\n" +
                        "Example: who");
}

WhoCommand.prototype = new Command();
WhoCommand.prototype.constructor = WhoCommand;

WhoCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var players = Realm.onlinePlayers();
    if (players.length === 1) {
        this.send("Only you are online.");
    } else {
        this.send("%1 are online.", players.joinFancy(Options.Capitalized));
    }
};

CommandRegistry.registerCommand("who", new WhoCommand());
