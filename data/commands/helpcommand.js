function HelpCommand() {
    Command.call(this);

    this.setDescription("Show in-game help, like the one you are now reading.\n" +
                        "\n" +
                        "Examples: help, help buy");
}

HelpCommand.prototype = new Command();
HelpCommand.prototype.constructor = HelpCommand;

HelpCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var m = "";

    if (hasWordsLeft()) {
        var commandName = takeWord();

        if (commandName === "commands") {
            var commandNames = CommandRegistry.commandNames().filter(function(commandName) {
                return !commandName.startsWith("/");
            });
            m = "\nHere is a list of all the commands you can use:\n\n" +
                Util.formatColumns(commandNames, Options.Highlighted) +
                "\nType *help <command>* to see help about a particular command.";

            if (player.admin) {
                m += "\nTo see all the admin commands you can use, type *help admin-commands*.";
            }
        } else {
            if (CommandRegistry.contains(commandName)) {
                m = "\n*" + commandName + "*\n  " +
                    Util.splitLines(CommandRegistry.description(commandName), 77)
                        .join("\n  ") + "\n\n";
            } else if (player.admin) {
                if (CommandRegistry.adminCommandsContains(commandName)) {
                    m = "\n*" + commandName + "*\n  " +
                        Util.splitLines(CommandRegistry.adminCommandDescription(commandName), 77)
                            .join("\n  ") + "\n\n";
                } else if (CommandRegistry.apiCommandsContains(commandName)) {
                    m = "\n*" + commandName + "*\n  " +
                        Util.splitLines(CommandRegistry.apiCommandDescription(commandName), 77)
                            .join("\n  ") + "\n\n";
                } else {
                    m = this.showAdminHelp(commandName);
                }
            }

            if (m.isEmpty()) {
                m = "The command \"%1\" is not recognized.\n" +
                    "Type *help commands* to see a list of all commands.".arg(commandName);
            }
        }
    } else {
        m = "\n" +
            "Type *help commands* to see a list of all commands.\n" +
            "Type *help <command>* to see help about a particular command.";
    }

    player.send(Util.processHighlights(m));
};

HelpCommand.prototype.showAdminHelp = function(commandName) {

    var m = "";

    if (commandName === "admin-commands") {
        m = "\n" +
            "Here is a list of all the commands you can use as an admin:\n" +
            "\n" +
            "*Remember: With great power comes great responsibility!*\n" +
            "\n" +
            Util.formatColumns(CommandRegistry.adminCommandNames(), Options.Highlighted) +
            "\n" +
            "Type *help <command>* to see help about a particular command.\n" +
            "Type *help admin-tips* to see some general tips for being an admin.";
    } else if (commandName === "admin-tips") {
        m = "\n" +
            "*Admin Tips*\n" +
            "\n" +
            "Now that you are an admin, you can actively modify the game world. Obviously, great " +
            "care should be taken, as many modifications are not revertable. Now, trusting you " +
            "will do the right thing, here as some tips for you:\n" +
            "\n" +
            "When referring to an object, you can use *#<id>* rather than referring to them by " +
            "name. For example, *set-prop #35 description This is the object with ID 35.*.\n" +
            "\n" +
            "Similarly, you can always use the word *room* to refer to the current room you are " +
            "in. But, to make editing of rooms even easier, you can use *@<property>* as a " +
            "shortcut for *get-* or *set-prop room <property>*. Thus, you can simply type: " +
            "*@id* to get the ID of the current room. Or, to set the description: *@description " +
            "As you stand just outside the South Gate, ...*\n" +
            "\n" +
            "Not listed in the admin commands overview, but very useful: *edit-prop* and " +
            "*edit-trigger* are available for more convenient editing of properties and " +
            "triggers. The usage is the same as for *get-prop* and *get-trigger*. Note that " +
            "these commands are only available if you use the web interface (not supported when " +
            "using telnet).\n" +
            "\n";
    } else if (commandName === "triggers") {
        m = "\n" +
            "*Trigger Overview*\n" +
            "\n" +
            "Here is a list of all the triggers which are available:\n" +
            "\n";
        TriggerRegistry.signatures().forEach(function(triggerName) {
            m += "  " + triggerName.highlighted() + "\n";
        });
        m += "\n" +
             "Type *help <trigger>* to see help about a particular trigger.\n";
    } else {
        TriggerRegistry.signatures().forEach(function(triggerName) {
            if (triggerName.startsWith(commandName)) {
                m = "\n" +
                    triggerName.highlighted() + "\n  " +
                    Util.splitLines(TriggerRegistry.description(triggerName), 77).join("\n  ") +
                    "\n\n";
            }
        });
    }

    return m;
};

CommandRegistry.registerCommand("help", new HelpCommand());
