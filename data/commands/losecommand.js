function LoseCommand() {
    Command.call(this);

    this.setDescription("Remove yourself or someone else from a group. If you are a group " +
                        "leader, you can remove anyone from your group by using *lose <name>*. " +
                        "You can always remove yourself from a group using simply *lose*.\n" +
                        "\n" +
                        "Examples: lose mia, lose");
}

LoseCommand.prototype = new Command();
LoseCommand.prototype.constructor = LoseCommand;

LoseCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (this.hasWordsLeft()) {
        if (this.peekWord() === "all") {
            player.disband();
            return;
        }

        var character = this.takeObject(this.currentRoom.characters);
        if (!this.requireSome(character, "Lose who?")) {
            return;
        }

        player.lose(character);
    } else {
        player.lose();
    }
}

CommandRegistry.registerCommand("lose", new LoseCommand());
