function ShoutCommand() {
    Command.call(this);

    this.setDescription("Shout something. You should shout when you want to be heard about " +
                        "greater distances.\n" +
                        "\n" +
                        "Example: shout Anyone there?");
}

ShoutCommand.prototype = new Command();
ShoutCommand.prototype.constructor = ShoutCommand;

ShoutCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Shout what?")) {
        return;
    }

    var message = takeRest();

    player.shout(message);
}

CommandRegistry.registerCommand("shout", new ShoutCommand());
