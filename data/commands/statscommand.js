function StatsCommand() {
    Command.call(this);

    this.setDescription("Display your character stats.\n" +
                        "\n" +
                        "Example: stats");
}

StatsCommand.prototype = new Command();
StatsCommand.prototype.constructor = StatsCommand;

StatsCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var stats = player.stats();

    player.send("\n" +
                Util.colorize("/===================== Overview ==================\\\n" +
                              "| %1 |\n".arg(Util.center("%1, a %2 %3"
                                                         .arg(player.name,
                                                              player.race.adjective,
                                                              player.characterClass.name))) +
                              "\\=================================================/\n",
                              Color.Green) +
                "\n" +
                Util.colorize(" /===== Physique ======\\   /==== Attributes =====\\ \n" +
                              " | Gender:  %1     |   | STR:  %2   DEX:  %3 | \n"
                              .arg(player.gender.leftJustified(6),
                                   stats[STRENGTH].toString().leftJustified(2),
                                   stats[DEXTERITY].toString().leftJustified(2)) +
                              " | Height:  %1      |   | VIT:  %2   END:  %3 | \n"
                              .arg(Util.formatHeight(player.height).leftJustified(5),
                                   stats[VITALITY].toString().leftJustified(2),
                                   stats[ENDURANCE].toString().leftJustified(2)) +
                              " | Weight:  %1      |   | INT:  %2   FAI:  %3 | \n"
                              .arg(Util.formatWeight(player.weight).leftJustified(5),
                                   stats[INTELLIGENCE].toString().leftJustified(2),
                                   stats[FAITH].toString().leftJustified(2)) +
                              " \\=====================/   \\=====================/ \n",
                              Color.Olive) +
             "\n");
};

CommandRegistry.registerCommand("stats", new StatsCommand());
