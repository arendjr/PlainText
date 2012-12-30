function AddItemCommand() {
    AdminCommand.call(this);

    this.setDescription("Add an item to the current room.\n" +
                        "\n" +
                        "Usage: add-item <item-name>");
}

AddItemCommand.prototype = new AdminCommand();
AddItemCommand.prototype.constructor = AddItemCommand;

AddItemCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Usage: add-item <item-name>")) {
        return;
    }

    var itemName = this.takeWord();

    var item = Realm.createObject("Item");
    item.name = itemName;
    this.currentRoom.addItem(item);

    this.send("Item %1 added.", itemName);
};

CommandRegistry.registerAdminCommand("add-item", new AddItemCommand());
