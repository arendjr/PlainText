function TellCommand() {
    Command.call(this);

    this.setDescription("Tell something to another online player (no need to be in the same " +
                        "room). Only this player will hear you.\n" +
                        "\n" +
                        "Example: tell earl Hey Earl, where are you?");
}

TellCommand.prototype = new Command();
TellCommand.prototype.constructor = TellCommand;

TellCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Tell who?")) {
        return;
    }

    var userName = this.takeWord().capitalized();
    var other = Realm.getPlayer(userName);
    if (!other || !other.isOnline()) {
        this.send("%1 is not online.", userName);
        return;
    }

    var message = this.takeRest();

    player.tell(other, message);

    other.lastTell = player;
};

CommandRegistry.registerCommand("tell", new TellCommand());
