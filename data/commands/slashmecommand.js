function SlashMeCommand() {
    Command.call(this);
}

SlashMeCommand.prototype = new Command();
SlashMeCommand.prototype.constructor = SlashMeCommand;

SlashMeCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Try being a little bit more creative...")) {
        return;
    }

    var action = this.takeRest();

    this.currentRoom.characters.send(Util.colorize("%1 %2".arg(player.name, action), Color.Purple));
};

CommandRegistry.registerCommand("/me", new SlashMeCommand());
