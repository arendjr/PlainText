function AddWeaponCommand() {
    AdminCommand.call(this);

    this.setDescription("Add a weapon to the current room.\n" +
                        "\n" +
                        "Usage: add-weapon <weapon-name>");
}

AddWeaponCommand.prototype = new AdminCommand();
AddWeaponCommand.prototype.constructor = AddWeaponCommand;

AddWeaponCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Usage: add-weapon <weapon-name>")) {
        return;
    }

    var weaponName = this.takeWord();

    var weapon = Realm.createObject("Weapon");
    weapon.name = weaponName;
    this.currentRoom.addItem(weapon);

    this.send("Weapon %1 added.", weaponName);
};

CommandRegistry.registerAdminCommand("add-weapon", new AddWeaponCommand());
