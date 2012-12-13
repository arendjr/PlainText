function ReplyCommand() {
    Command.call(this);

    this.setDescription("Reply to a player who recently told you something. Only this player " +
                        "will hear you.\n" +
                        "\n" +
                        "Example: reply I'm north of you! I can see you!");
}

ReplyCommand.prototype = new Command();
ReplyCommand.prototype.constructor = ReplyCommand;

ReplyCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!player.lastTell) {
        this.send("You have not received a tell recently.");
        return;
    }

    var other = player.lastTell;
    if (!other.isOnline()) {
        this.send("%1 is not online.", other.name);
        return;
    }

    var message = this.takeRest();

    player.tell(other, message);

    other.lastTell = player;
};

CommandRegistry.registerCommand("reply", new ReplyCommand());
