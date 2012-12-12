function WieldCommand() {
    Command.call(this);

    this.setDescription("Wield a weapon or shield that's in your inventory.\n" +
                        "\n" +
                        "Example: wield sword");
}

WieldCommand.prototype = new Command();
WieldCommand.prototype.constructor = WieldCommand;

WieldCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    this.takeWord("my");

    var item = this.takeObject(player.inventory);
    if (!this.requireSome(item, "Wield what?")) {
        return;
    }

    player.wield(item);
}

CommandRegistry.registerCommand("wield", new WieldCommand());
