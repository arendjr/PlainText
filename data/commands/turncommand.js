function TurnCommand() {
    Command.call(this);

    this.setDescription("Turn to face some other direction.\n" +
                        "\n" +
                        "Examples: turn left, turn right, turn around");
}

TurnCommand.prototype = new Command();
TurnCommand.prototype.constructor = TurnCommand;

TurnCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var direction = this.takeWord();

    if (direction === "left") {
        player.direction = [player.direction[1], -player.direction[0], player.direction[2]];
    } else if (direction === "right") {
        player.direction = [-player.direction[1], player.direction[0], player.direction[2]];
    } else if (direction === "around") {
        player.direction = [-player.direction[0], -player.direction[1], -player.direction[2]];
    } else {
        this.send("Turn where?");
        return;
    }

    player.send("You turn %1.".arg(direction));
    player.lookAhead();
};

CommandRegistry.registerCommand("turn", new TurnCommand());
