function PutCommand() {
    Command.call(this);

    this.setDescription("Put an item from your inventory into a container.\n" +
                        "\n" +
                        "Example: put stick in bag");
}

PutCommand.prototype = new Command();
PutCommand.prototype.constructor = PutCommand;

PutCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var items = this.takeObjects(player.inventory);
    if (!this.requireSome(items, "Put what?")) {
        return;
    }

    this.takeWord(/in(to)?/);

    var container = this.takeObject(player.inventory);
    if (!this.requireSome(container, "Put where?")) {
        return;
    }
    if (!container.isContainer()) {
        this.send("%1 is not a container.", container.definiteName(player.inventory,
                                                                   Options.Capitalized));
        return;
    }

    items.removeOne(container);
    if (items.isEmpty()) {
        this.send("Put what?");
        return;
    }

    items.forEach(function(item) {
        container.addItem(item);
        player.removeInventoryItem(item);
    });

    this.send("You put %1 in %2.", items.joinFancy(Options.DefiniteArticles),
              container.definiteName(player.inventory));
}

CommandRegistry.registerCommand("put", new PutCommand());
