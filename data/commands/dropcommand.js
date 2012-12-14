function DropCommand() {
    Command.call(this);

    this.setDescription("Drop an item or money from your inventory.\n" +
                        "\n" +
                        "Example: drop stick");
}

DropCommand.prototype = new Command();
DropCommand.prototype.constructor = DropCommand;

DropCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var description;
    var items = [];
    var gold = 0.0;

    var word = this.peekWord();
    if (word.startsWith("$")) {
        var currencyRegExp = /^\$\d+(\.5)?$/;
        if (!currencyRegExp.test(word)) {
            this.send("%1 is not a valid currency description.", word);
            return;
        }
        gold = word.mid(1).toDouble();
        if (gold == 0.0) {
            this.send("You drop nothing.");
            return;
        }
        if (gold > player.gold) {
            this.send("You don't have that much gold.");
            return;
        }

        this.currentRoom.addGold(gold);
        this.player.gold -= gold;

        description = "$%1 worth of gold".arg(gold);

        this.takeWord();
    } else {
        items = this.takeObjects(player.inventory);
        if (!this.requireSome(items, "Drop what?")) {
            return;
        }

        items.forEach(function(item) {
            player.currentRoom.addItem(item);
            player.removeInventoryItem(item);
        });

        description = items.joinFancy(Options.DefiniteArticles);
    }

    this.send("You drop %1.", description);

    var others = this.currentRoom.characters;
    others.removeOne(player);
    others.send("%1 drops %2.".arg(player.name, description));
};

CommandRegistry.registerCommand("drop", new DropCommand());
