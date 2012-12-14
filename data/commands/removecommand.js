function RemoveCommand() {
    Command.call(this);

    this.setDescription("Remove an item that is currently worn or wielded and put it back in " +
                        "your inventory.\n" +
                        "\n" +
                        "Example: remove sword");
}

RemoveCommand.prototype = new Command();
RemoveCommand.prototype.constructor = RemoveCommand;

RemoveCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Remove what?")) {
        return;
    }

    this.takeWord("my");

    var allItems = [];
    if (player.weapon) {
        allItems.append(player.weapon);
    }
    if (player.secondaryWeapon) {
        allItems.append(player.secondaryWeapon);
    }
    if (player.shield) {
        allItems.append(player.shield);
    }

    var item = this.takeObject(allItems);
    if (!this.requireSome(item, "You're not wearing or wielding that.")) {
        return;
    }

    player.remove(item);
};

CommandRegistry.registerCommand("remove", new RemoveCommand());
