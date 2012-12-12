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
    if (this.weapon) {
        allItems.append(this.weapon);
    }
    if (this.secondaryWeapon) {
        allItems.append(this.secondaryWeapon);
    }
    if (this.shield) {
        allItems.append(this.shield);
    }

    var item = this.takeObject(allItems);
    if (!this.requireSome(item, "You're not wearing or wielding that.")) {
        return;
    }

    player.remove(item);
}

CommandRegistry.registerCommand("remove", new RemoveCommand());
