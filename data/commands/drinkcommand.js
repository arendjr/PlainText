function DrinkCommand() {
    Command.call(this);

    this.setDescription("Drink an item in the current room or your inventory.\n" +
                        "\n" +
                        "Example: drink potion");
}

DrinkCommand.prototype = new Command();
DrinkCommand.prototype.constructor = DrinkCommand;

DrinkCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var allItems = player.inventory.concat(player.currentRoom.items);
    var item = this.takeObject(allItems);
    if (!this.requireSome(item, "Drink what?")) {
        return;
    }

    if (!item.hasTrigger("ondrink")) {
        player.send("You cannot drink that.");
        return;
    }

    if (!item.invokeTrigger("ondrink", player)) {
        return;
    }

    item.setDeleted();
}

CommandRegistry.registerCommand("drink", new DrinkCommand());
