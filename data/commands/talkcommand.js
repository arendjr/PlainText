function TalkCommand() {
    Command.call(this);

    this.setDescription("Talk to a specific character who's in the same room. Everyone close to " +
                        "you can hear it.\n" +
                        "\n" +
                        "Example: talk earl Hey Earl, how are you?");
}

TalkCommand.prototype = new Command();
TalkCommand.prototype.constructor = TalkCommand;

TalkCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var character = this.takeObject(player.currentRoom.characters);
    if (!this.requireSome(character, "Talk to who?")) {
        return;
    }

    if (!this.assertWordsLeft("Say what?")) {
        return;
    }

    var message = this.takeRest();

    player.talk(character, message);
};

CommandRegistry.registerCommand("talk", new TalkCommand());
