function DescriptionCommand() {
    Command.call(this);

    this.setDescription("Change the description of your own character (max. 140 characters). " +
                        "Others will see this description when they look at you.\n" +
                        "\n" +
                        "Example: description Wearing his axe on his back, he looks a rather " +
                        "fearsome warrior.");
}

DescriptionCommand.prototype = new Command();
DescriptionCommand.prototype.constructor = DescriptionCommand;

DescriptionCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("What should your description be?")) {
        return;
    }

    var description = this.takeRest().left(140).capitalized();
    if (!description.endsWith(".") && !description.endsWith("!") && !description.endsWith("?")) {
        description += ".";
    }

    player.description = description;

    player.send("Your description has been changed to:\n" + description);
};

CommandRegistry.registerCommand("description", new DescriptionCommand());
