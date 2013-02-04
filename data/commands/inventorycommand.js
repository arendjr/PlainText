function InventoryCommand() {
    Command.call(this);

    this.setDescription("View your current inventory.\n" +
                        "\n" +
                        "Example: inventory");
}

InventoryCommand.prototype = new Command();
InventoryCommand.prototype.constructor = InventoryCommand;

InventoryCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var weight = player.inventoryWeight();
    var carriedInventoryString;
    if (player.inventory.isEmpty()) {
        carriedInventoryString = "You don't carry anything.\n";
    } else {
        if (weight === 0) {
            carriedInventoryString = "You carry %1.\n"
                                     .arg(player.inventory.joinFancy());
        } else if (player.inventory.length === 1) {
            carriedInventoryString = "You carry %1, weighing %2.\n"
                                     .arg(player.inventory[0].indefiniteName(),
                                          Util.formatWeight(weight));
        } else {
            carriedInventoryString = "You carry %1, weighing a total of %2.\n"
                                     .arg(player.inventory.joinFancy(), Util.formatWeight(weight));
        }
    }

    var carriedGoldString;
    if (player.gold === 0.0) {
        carriedGoldString = "You don't have any gold.\n";
    } else {
        carriedGoldString = "You've got $%1 worth of gold.\n".arg(player.gold);
    }

    player.send(carriedInventoryString + carriedGoldString);
};

CommandRegistry.registerCommand("inventory", new InventoryCommand());
