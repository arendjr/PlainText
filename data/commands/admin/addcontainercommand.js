function AddContainerCommand() {
    AdminCommand.call(this);

    this.setDescription("Add a container to the current room.\n" +
                        "\n" +
                        "Usage: add-container <container-name>");
}

AddContainerCommand.prototype = new AdminCommand();
AddContainerCommand.prototype.constructor = AddContainerCommand;

AddContainerCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Usage: add-container <container-name>")) {
        return;
    }

    var containerName = this.takeWord();

    var container = Realm.createObject("Container");
    container.name = containerName;
    container.portable = true;
    this.currentRoom.addItem(container);

    this.send("Container %1 added.", containerName);
};

CommandRegistry.registerAdminCommand("add-container", new AddContainerCommand());
