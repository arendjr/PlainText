function AddShieldCommand() {
    AdminCommand.call(this);

    this.setDescription("Add a shield to the current room.\n" +
                        "\n" +
                        "Usage: add-shield <shield-name>");
}

AddShieldCommand.prototype = new AdminCommand();
AddShieldCommand.prototype.constructor = AddShieldCommand;

AddShieldCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Usage: add-shield <shield-name>")) {
        return;
    }

    var shieldName = this.takeWord();

    var shield = Realm.createObject("Shield");
    shield.name = shieldName;
    this.currentRoom.addItem(shield);

    this.send("Shield %1 added.", shieldName);
};

CommandRegistry.registerAdminCommand("add-shield", new AddShieldCommand());
