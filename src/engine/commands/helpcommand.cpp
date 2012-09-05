#include "helpcommand.h"

#include "scriptengine.h"
#include "util.h"


HelpCommand::HelpCommand(Player *character, const QMap<QString, Command *> &commands,
                         QObject *parent) :
    Command(character, parent),
    m_commands(commands) {

    setDescription("Show in-game help, like the one you are now reading.\n"
                   "\n"
                   "Examples: help, help buy");
}

HelpCommand::~HelpCommand() {
}

void HelpCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    QString m;

    if (hasWordsLeft()) {
        QString commandName = takeWord();

        if (commandName == "commands") {
            QStringList commandNames;
            for (const QString &commandName : m_commands.keys()) {
                if (!commandName.contains("-")) {
                    commandNames << commandName;
                }
            }

            m = "\n"
                "Here is a list of all the commands you can use:\n"
                "\n" +
                formatColumns(commandNames) +
                "\n"
                "Type *help <command>* to see help about a particular command.";

            if (player()->isAdmin()) {
                m += "\n"
                     "To see all the admin commands you can use, type *help admin-commands*.";
            }
        } else {
            if (m_commands.contains(commandName)) {
                m = "\n" +
                    Util::highlight(commandName) + "\n  " +
                    Util::splitLines(m_commands[commandName]->description(), 78).join("\n  ") +
                    "\n\n";
            } else if (player()->isAdmin()) {
                m = showAdminHelp(commandName);
            }

            if (m.isEmpty()) {
                m = QString("The command \"%1\" is not recognized.\n"
                            "Type *help commands* to see a list of all commands.").arg(commandName);
            }
        }
    } else {
        m = "\n"
            "Type *help commands* to see a list of all commands.\n"
            "Type *help <command>* to see help about a particular command.";
    }

    send(Util::processHighlights(m));
}

QString HelpCommand::showAdminHelp(const QString &commandName) {

    QString m;

    if (commandName == "admin-commands") {
        QStringList commandNames;
        for (const QString &commandName : m_commands.keys()) {
            if (commandName.contains("-") && !commandName.startsWith("api-")) {
                commandNames << commandName;
            }
        }

        m = "\n"
            "Here is a list of all the commands you can use as an admin:\n"
            "\n"
            "*Remember: With great power comes great responsibility!*\n"
            "\n" +
            formatColumns(commandNames) +
            "\n"
            "Type *help <command>* to see help about a particular command.\n"
            "Type *help admin-tips* to see some general tips for being an admin.";

    } else if (commandName == "admin-tips") {

        m = "\n"
            "*Admin Tips*\n"
            "\n"
            "Now that you are an admin, you can actively modify the game world. Obviously, great "
            "care should be taken, as many modifications are not revertable. Now, trusting you "
            "will do the right thing, here as some tips for you:\n"
            "\n"
            "When referring to an object, you can use *#<id>* rather than referring to them by "
            "name. For example, *set-prop #35 description This is the object with ID 35.*.\n"
            "\n"
            "Similarly, you can always use the word *area* to refer to the current area you are "
            "in. But, to make editing of areas even easier, you can use *@<property>* as a "
            "shortcut for *get-* or *set-prop area <property>*. Thus, you can simply type: "
            "*@id* to get the ID of the current area. Or, to set the description: *@description As "
            "you stand just outside the South Gate, ...*\n"
            "\n"
            "Not listed in the admin commands overview, but very useful: *edit-prop* and "
            "*edit-trigger* are available for more convenient editing of properties and triggers. "
            "The usage is the same as for *get-prop* and *get-trigger*. Note that these commands "
            "are only available if you use the web interface (not supported when using telnet).\n"
            "\n";

    } else if (commandName == "triggers") {

        m = "\n"
            "*Triggers*\n"
            "\n"
            "Writing triggers is pretty straightforward, but there are a few things you should "
            "know. Most importantly, any trigger is just a JavaScript function. So if you know "
            "JavaScript, you will be able to write triggers just fine. Still, there are some "
            "differences from writing JavaScript in a webbrowser of course. Here's an example of a "
            "very basic trigger:\n"
            "\n"
            "    function() {\n"
            "        this.go(this.currentArea.exits.named('north'));\n"
            "    }\n"
            "\n"
            "The above example is a trigger that works on a character, and will let him walk "
            "north, if possible.\n"
            "\n"
            "*Properties and methods*\n"
            "\n"
            "While writing some trigger you will often be interested to know which properties and "
            "methods are available on some object. In the example above, go() is a method that's "
            "available on character objects, currentArea is a property available on character "
            "objects, and exits is a property available on area objects. To inspect an object and "
            "list its properties or methods, use the *list-props* or *list-methods* commands, "
            "respectively.\n"
            "\n"
            "*Timers*\n"
            "\n"
            "Ever so often, you will want to write a trigger that performs some action in a "
            "delayed fashion. Possibly you will want to have that action be repeated at an "
            "interval. For these purposes, there are setTimeout() and setInterval() functions "
            "which work just like those in your webbrowser, with one difference only: The "
            "functions are attached to any game object rather than to the global window object as "
            "you may be used to. So, to make the character from above walk north with a delay of "
            "half a second, we could write this trigger:\n"
            "\n"
            "    function() {\n"
            "        this.setTimeout(function() {\n"
            "            this.go(this.currentArea.exits.named('north'));\n"
            "        }, 500);\n"
            "    }\n"
            "\n"
            "*Return values*\n"
            "\n"
            "In many cases, triggers have the ability to cancel the action that triggered them. "
            "Whenever this is the case, you can have your trigger return the value false, and the "
            "action will be canceled. Canceling the action is supported by any trigger that lists "
            "the bool return type in the overview given below. For example, if the following "
            "function is attached to an item's onopen trigger, the item cannot be opened:\n"
            "\n"
            "    function(activator) {\n"
            "        activator.send('The lid appears to be stuck.');\n"
            "        return false;\n"
            "    }\n"
            "\n"
            "*Testing scripts*\n"
            "\n"
            "If you simply want to test whether some script works as expected, you can use the "
            "*exec-script* command. In addition to testing, this command is also useful to clean "
            "up game data and objects when some script had unintended side-effects. If you want to "
            "execute some script on a specific object, you can use this function for getting the "
            "object: *$('<object-type>:<object-id>')*. Example:\n"
            "\n"
            "    exec-script $('character:167').say('Miauw')\n"
            "\n"
            "*Trigger Overview*\n"
            "\n"
            "Here is a list of all the triggers which are available:\n"
            "\n";
        for (const QString &triggerName : ScriptEngine::triggers().keys()) {
            m += "  " + Util::highlight(triggerName) + "\n";
        }
        m += "\n"
             "Type *help <trigger>* to see help about a particular trigger.\n";

    } else {
        for (const QString &triggerName : ScriptEngine::triggers().keys()) {
            if (triggerName.startsWith(commandName)) {
                m = "\n" +
                    Util::highlight(triggerName) + "\n  " +
                    Util::splitLines(ScriptEngine::triggers()[triggerName], 78).join("\n  ") +
                    "\n\n";
                break;
            }
        }
    }

    return m;
}

QString HelpCommand::formatColumns(const QStringList &commandNames) {

    QString message;

    int length = commandNames.length();
    int halfLength = length / 2 + length % 2;
    for (int i = 0; i < halfLength; i++) {
        QString first = commandNames[i];
        QString second = i + halfLength < length ? commandNames[i + halfLength] : "";

        message += "  " + Util::highlight(first.leftJustified(30)) +
                   "  " + Util::highlight(second) + "\n";
    }

    return message;
}
