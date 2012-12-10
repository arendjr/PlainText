function GetCommand() {
    Command.call(this);

    this.setDescription("Take an item or gold from the current area or a container and put it in " +
                        "your inventory.\n" +
                        "\n" +
                        "Examples: get stick, take stick, get gold, get stick from bag");
}

GetCommand.prototype = new Command();
GetCommand.prototype.constructor = GetCommand;

GetCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var description = this.takeObjectsDescription();

    var items;
    if (!this.takeWord("from").isEmpty()) {
        var container = this.takeObject(player.inventory);
        if (!this.requireSome(container, "%1 from what?".arg(this.alias.capitalized()))) {
            return;
        }
        if (!container.isContainer()) {
            this.send("%1 is not a container", container.definiteName(player.inventory,
                                                                      Options.Capitalized));
            return;
        }

        items = this.objectsByDescription(description, container.items);
        if (!this.requireSome(items, "%1 what?".arg(this.alias.capitalized()))) {
            return;
        }

        items.forEach(function(item) {
            player.addInventoryItem(item);
            container.removeItem(item);
        });

        this.send("You take %1 from %2.", items.joinFancy(Options.DefiniteArticles),
                  container.definiteName(player.inventory));
    } else {
        items = this.objectsByDescription(description, this.currentRoom.items);
        if (!this.requireSome(items, "%1 what?".arg(this.alias.capitalized()))) {
            return;
        }

        player.take(items);
    }
}

CommandRegistry.registerCommand("get", new GetCommand());
CommandRegistry.registerCommand("take", new GetCommand());
