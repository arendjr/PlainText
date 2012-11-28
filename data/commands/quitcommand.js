function QuitCommand() {
    Command.call(this);

    this.setDescription("Quit the game. Your progress and current location will be saved.\n" +
                        "\n" +
                        "Examples: quit, goodbye");
}

QuitCommand.prototype = new Command();
QuitCommand.prototype.constructor = QuitCommand;

QuitCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    player.quit();
}

CommandRegistry.registerCommand("goodbye", new QuitCommand());
CommandRegistry.registerCommand("quit", new QuitCommand());
