function EquipmentCommand() {
    Command.call(this);

    this.setDescription("View any equipment you are currently wielding.\n" +
                        "\n" +
                        "Example: equipment");
}

EquipmentCommand.prototype = new Command();
EquipmentCommand.prototype.constructor = EquipmentCommand;

EquipmentCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var wieldedEquipment = [];
    if (player.weapon !== null) {
        wieldedEquipment.append(player.weapon);
    }
    if (player.secondaryWeapon !== null) {
        wieldedEquipment.append(player.secondaryWeapon);
    }
    if (player.shield !== null) {
        wieldedEquipment.append(player.shield);
    }

    if (wieldedEquipment.isEmpty()) {
        player.send("You're not wielding anything.");
    } else {
        player.send("You're wielding %1.".arg(wieldedEquipment.joinFancy()));
    }
}

CommandRegistry.registerCommand("equipment", new EquipmentCommand());
