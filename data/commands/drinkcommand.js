function DrinkCommand() {

    this.setDescription("Drink an item in the current area or your inventory.\n" +
                        "\n" +
                        "Example: drink potion");
}

DrinkCommand.prototype = new Command();
DrinkCommand.prototype.constructor = DrinkCommand;

DrinkCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Drink what?")) {
        return;
    }

    this.takeWord("the");

    var allItems = player.inventory.concat(player.currentArea.items);
    var item = this.takeObject(allItems);
    if (!this.requireSome(item, "You don't have that.")) {
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
