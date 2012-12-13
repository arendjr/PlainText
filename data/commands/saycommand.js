function SayCommand() {
    Command.call(this);

    this.setDescription("Say something. Everyone close to you can hear it.\n" +
                        "\n" +
                        "Example: say How are you?");
}

SayCommand.prototype = new Command();
SayCommand.prototype.constructor = SayCommand;

SayCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Say what?")) {
        return;
    }

    var message = this.takeRest();

    player.say(message);
};

CommandRegistry.registerCommand("say", new SayCommand());
