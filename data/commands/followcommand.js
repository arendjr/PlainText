function FollowCommand() {
    Command.call(this);

    this.setDescription("Form or join a group by following another player. The first person " +
                        "being followed automatically becomes the group leader.\n" +
                        "\n" +
                        "Example: follow mia");
}

FollowCommand.prototype = new Command();
FollowCommand.prototype.constructor = FollowCommand;

FollowCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var character = this.takeObject(this.currentRoom.characters);
    if (!this.requireSome(character, "Follow who?")) {
        return;
    }

    player.follow(character);
};

CommandRegistry.registerCommand("follow", new FollowCommand());
