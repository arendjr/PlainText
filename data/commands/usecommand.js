function UseCommand() {
    Command.call(this);

    this.setDescription("Use an item from the current room or your inventory.\n" +
                        "\n" +
                        "Example: use key");
}

UseCommand.prototype = new Command();
UseCommand.prototype.constructor = UseCommand;

UseCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var pool = player.inventory.concat(player.currentRoom.items);
    var item = this.takeObject(pool);
    if (!this.requireSome(item, "Use what?")) {
        return;
    }

    if (!item.hasTrigger("onuse")) {
        this.send("You cannot use that.");
        return;
    }

    item.invokeTrigger("onuse", player);
};

CommandRegistry.registerCommand("use", new UseCommand());
