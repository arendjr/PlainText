function EatCommand() {
    Command.call(this);

    this.setDescription("Eat an item in the current room or your inventory.\n" +
                        "\n" +
                        "Example: eat cake");
}

EatCommand.prototype = new Command();
EatCommand.prototype.constructor = EatCommand;

EatCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var pool = player.inventory.concat(player.currentRoom.items);
    var item = this.takeObject(pool);
    if (!this.requireSome(item, "Eat what?")) {
        return;
    }

    if (!item.hasTrigger("oneat")) {
        player.send("You cannot eat that.");
        return;
    }

    if (!item.invokeTrigger("oneat", player)) {
        return;
    }

    item.setDeleted();
};

CommandRegistry.registerCommand("eat", new EatCommand());
