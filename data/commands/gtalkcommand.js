function GtalkCommand() {
    Command.call(this);

    this.setDescription("Talk to everyone who's in the same group.\n" +
                        "\n" +
                        "Example: gtalk Where do we go now?");
}

GtalkCommand.prototype = new Command();
GtalkCommand.prototype.constructor = GtalkCommand;

GtalkCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Say what?")) {
        return;
    }

    if (player.group) {
        var message = this.takeRest();
        var text = (message.endsWith(".") || message.endsWith("?") || message.endsWith("!")) ?
                   "(Group) %1 mentioned, \"%2\"" : "(Group) %1 mentioned, \"%2.\"";

        player.group.send(text.arg(player.name, message));
    } else {
        this.send("You are not in a group.");
    }
}

CommandRegistry.registerCommand("gtalk", new GtalkCommand());
