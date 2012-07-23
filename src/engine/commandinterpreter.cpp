#include "commandinterpreter.h"

#include <QRegExp>
#include <QStringList>

#include "area.h"
#include "exit.h"
#include "gameexception.h"
#include "gameobjectptr.h"
#include "player.h"
#include "realm.h"
#include "util.h"
#include "commands/buycommand.h"
#include "commands/closecommand.h"
#include "commands/descriptioncommand.h"
#include "commands/drinkcommand.h"
#include "commands/dropcommand.h"
#include "commands/eatcommand.h"
#include "commands/getcommand.h"
#include "commands/givecommand.h"
#include "commands/gocommand.h"
#include "commands/inventorycommand.h"
#include "commands/killcommand.h"
#include "commands/lookcommand.h"
#include "commands/opencommand.h"
#include "commands/quitcommand.h"
#include "commands/saycommand.h"
#include "commands/shoutcommand.h"
#include "commands/slashmecommand.h"
#include "commands/statscommand.h"
#include "commands/talkcommand.h"
#include "commands/tellcommand.h"
#include "commands/usecommand.h"
#include "commands/whocommand.h"
#include "commands/admin/addcharactercommand.h"
#include "commands/admin/addexitcommand.h"
#include "commands/admin/additemcommand.h"
#include "commands/admin/addshieldcommand.h"
#include "commands/admin/addweaponcommand.h"
#include "commands/admin/copyitemcommand.h"
#include "commands/admin/execscriptcommand.h"
#include "commands/admin/getpropcommand.h"
#include "commands/admin/gettriggercommand.h"
#include "commands/admin/listmethodscommand.h"
#include "commands/admin/listpropscommand.h"
#include "commands/admin/removeexitcommand.h"
#include "commands/admin/removeitemcommand.h"
#include "commands/admin/setclasscommand.h"
#include "commands/admin/setpropcommand.h"
#include "commands/admin/setracecommand.h"
#include "commands/admin/settriggercommand.h"
#include "commands/admin/stopservercommand.h"
#include "commands/admin/unsettriggercommand.h"


CommandInterpreter::CommandInterpreter(Player *player) :
    QObject(),
    m_player(player) {

    Command *get = new GetCommand(player, this);
    Command *go = new GoCommand(player, this);
    Command *kill = new KillCommand(player, this);
    Command *look = new LookCommand(player, this);
    Command *quit = new QuitCommand(player, this);

    m_commands.insert("attack", kill);
    m_commands.insert("buy", new BuyCommand(player, this));
    m_commands.insert("close", new CloseCommand(player, this));
    m_commands.insert("description", new DescriptionCommand(player, this));
    m_commands.insert("drink", new DrinkCommand(player, this));
    m_commands.insert("drop", new DropCommand(player, this));
    m_commands.insert("eat", new EatCommand(player, this));
    m_commands.insert("enter", go);
    m_commands.insert("get", get);
    m_commands.insert("give", new GiveCommand(player, this));
    m_commands.insert("go", go);
    m_commands.insert("goodbye", quit);
    m_commands.insert("inventory", new InventoryCommand(player, this));
    m_commands.insert("kill", kill);
    m_commands.insert("l", look);
    m_commands.insert("look", look);
    m_commands.insert("open", new OpenCommand(player, this));
    m_commands.insert("quit", quit);
    m_commands.insert("say", new SayCommand(player, this));
    m_commands.insert("shout", new ShoutCommand(player, this));
    m_commands.insert("stats", new StatsCommand(player, this));
    m_commands.insert("take", get);
    m_commands.insert("talk", new TalkCommand(player, this));
    m_commands.insert("tell", new TellCommand(player, this));
    m_commands.insert("use", new UseCommand(player, this));
    m_commands.insert("who", new WhoCommand(player, this));
    m_commands.insert("/me", new SlashMeCommand(player, this));

    if (m_player->isAdmin()) {
        m_commands.insert("add-character", new AddCharacterCommand(player, this));
        m_commands.insert("add-exit", new AddExitCommand(player, this));
        m_commands.insert("add-item", new AddItemCommand(player, this));
        m_commands.insert("add-shield", new AddShieldCommand(player, this));
        m_commands.insert("add-weapon", new AddWeaponCommand(player, this));
        m_commands.insert("copy-item", new CopyItemCommand(player, this));
        m_commands.insert("exec-script", new ExecScriptCommand(player, this));
        m_commands.insert("get-prop", new GetPropCommand(player, this));
        m_commands.insert("get-trigger", new GetTriggerCommand(player, this));
        m_commands.insert("list-methods", new ListMethodsCommand(player, this));
        m_commands.insert("list-props", new ListPropsCommand(player, this));
        m_commands.insert("remove-exit", new RemoveExitCommand(player, this));
        m_commands.insert("remove-item", new RemoveItemCommand(player, this));
        m_commands.insert("set-class", new SetClassCommand(player, this));
        m_commands.insert("set-prop", new SetPropCommand(player, this));
        m_commands.insert("set-race", new SetRaceCommand(player, this));
        m_commands.insert("set-trigger", new SetTriggerCommand(player, this));
        m_commands.insert("stop-server", new StopServerCommand(player, this));
        m_commands.insert("unset-trigger", new UnsetTriggerCommand(player, this));

        m_triggers.insert("onactive : void",
                          "The onactive trigger is invoked on any character when it's no longer "
                          "stunned, ie. when it can perform a new action again.");
        m_triggers.insert("onattack(attacker : character) : bool",
                          "The onattack trigger is invoked on any character when it's being "
                          "attacked.");
        m_triggers.insert("onbuy(buyer : character, boughtItem : optional item) : bool",
                          "The onbuy trigger is invoked on any character when something is being "
                          "bought from it. When boughtItem is omitted, the buyer is requesting an "
                          "overview of the things for sale.");
        m_triggers.insert("oncharacterattacked(attacker : character, defendant : character) : void",
                          "The oncharacterattacked trigger is invoked on any character in an area, "
                          "except for the attacker and defendant themselves, when another "
                          "character in that area emerges into combat.");
        m_triggers.insert("oncharacterdied(defendant : character, attacker : optional character) : "
                          "bool",
                          "The oncharacterdied trigger is invoked on any character in an area, "
                          "when another character in that area dies. When attacker is omitted, the "
                          "defendant died because of a non-combat cause (for example, poison).");
        m_triggers.insert("oncharacterentered(activator : character) : void",
                          "The oncharacterentered trigger is invoked on any character in an area "
                          "when another character enters that area.");
        m_triggers.insert("onclose(activator : character) : bool",
                          "The onclose trigger is invoked on any item or exit when it's closed.");
        m_triggers.insert("ondie(attacker : optional character) : bool",
                          "The ondie trigger is invoked on any character when it dies. When "
                          "attacker is omitted, the character died because of a non-combat cause "
                          "(for example, poison).");
        m_triggers.insert("ondrink(activator : character) : bool",
                          "The ondrink trigger is invoked on any item when it's drunk.");
        m_triggers.insert("oneat(activator : character) : bool",
                          "The ondrink trigger is invoked on any item when it's eaten.");
        m_triggers.insert("onenter(activator : character) : bool",
                          "The onenter trigger is invoked on any exit when it's entered.");
        m_triggers.insert("oncharacterexit(activator : character, exitName : string) : bool",
                          "The onexit trigger is invoked on any character in an area when another "
                          "character leaves that area.");
        m_triggers.insert("oninit : void",
                          "The oninit trigger is invoked once on every object when the game server "
                          "is started. Note: For characters that do have an onspawn trigger, but "
                          "no oninit trigger, onspawn is triggered instead.");
        m_triggers.insert("onopen(activator : character) : bool",
                          "The onopen trigger is invoked on any item or exit when it's opened.");
        m_triggers.insert("onreceive(giver : character, item : item or item list or amount) : bool",
                          "The onreceive trigger is invoked on any character when something is "
                          "being given to it. Note that item may be a number instead of an item "
                          "object when an amount of gold is being given, or a list of items when "
                          "multiple items are being given.");
        m_triggers.insert("onspawn : void",
                          "The onspawn trigger is invoked on any character when it respawns.");
        m_triggers.insert("ontalk(speaker : character, message : string) : void",
                          "The ontalk trigger is invoked on any character when talked to.");
        m_triggers.insert("onuse(activator : character) : void",
                          "The onuse trigger is invoked on any item when it's used.");
    }

    connect(quit, SIGNAL(quit()), this, SIGNAL(quit()));
}

CommandInterpreter::~CommandInterpreter() {
}

void CommandInterpreter::execute(const QString &command) {

    try {
        QStringList words = command.trimmed().split(QRegExp("\\s+"));
        QString commandName = words[0].toLower();
        if (commandName.isEmpty()) {
            return;
        }

        if (commandName == "help") {
            if (words.length() > 1) {
                showHelp(words[1].toLower());
            } else {
                showHelp();
            }
            return;
        }

        if (Util::isDirectionAbbreviation(commandName)) {
            words[0] = Util::direction(commandName);
            commandName = words[0];
        }
        if (Util::isDirection(commandName) || commandName == "out") {
            words.prepend("go");
            m_commands["go"]->execute(words.join(" "));
            return;
        }

        if (m_commands.contains(commandName)) {
            m_commands[commandName]->execute(command);
            return;
        }

        QList<Command *> commands;
        for (const QString &key : m_commands.keys()) {
            if (key.startsWith(commandName)) {
                commands << m_commands[key];
            }
        }

        if (commands.length() == 1) {
            commands[0]->execute(command);
        } else if (commands.length() > 1) {
            m_player->send("Command is not unique.");
        } else {
            m_player->send(QString("Command \"%1\" does not exist.").arg(words[0]));
        }
    } catch (GameException &exception) {
        m_player->send(QString("Executing the command gave an exception: "
                               "%1").arg(exception.what()));
        if (!m_player->isAdmin()) {
            m_player->send("This is not good. You may want to contact a game admin about this.");
        }
    } catch (...) {
        m_player->send("Executing the command gave an unknown exception.");
        if (!m_player->isAdmin()) {
            m_player->send("This is not good. You may want to contact a game admin about this.");
        }
    }
}

void CommandInterpreter::showHelp(const QString &command) {

    QString m;

    if (command.isEmpty()) {

        m = "\n"
            "Type *help commands* to see a list of all commands.\n"
            "Type *help <command>* to see help about a particular command.";

    } else if (command == "commands") {
        QStringList commandNames = m_commands.keys();

        m = "\n"
            "Here is a list of all the commands you can use:\n"
            "\n" +
            formatColumns(commandNames.filter(QRegExp("^\\w+$"))) +
            "\n"
            "Type *help <command>* to see help about a particular command.";

        if (m_player->isAdmin()) {
            m += "\n"
                 "To see all the admin commands you can use, type *help admin-commands*.";
        }
    } else {
        if (m_commands.contains(command)) {

            m = "\n" +
                Util::highlight(command) + "\n"
                "  " + Util::splitLines(m_commands[command]->description(), 78).join("\n  ") +
                "\n\n";

        } else if (m_player->isAdmin()) {
            m = showAdminHelp(command);
        }

        if (m.isEmpty()) {
            m = QString("The command \"%1\" is not recognized.\n"
                        "Type *help commands* to see a list of all commands.").arg(command);
        }
    }

    static QRegExp bold("\\*([\\w \"'<>():@#$!.,-]+)\\*");
    int pos = 0;
    while ((pos = bold.indexIn(m)) != -1) {
        m = m.replace(pos, bold.matchedLength(), Util::highlight(bold.cap(1)));
    }

    m_player->send(m);
}

QString CommandInterpreter::showAdminHelp(const QString &command) {

    QString m;

    if (command == "admin-commands") {
        QStringList commandNames = m_commands.keys();

        m = "\n"
            "Here is a list of all the commands you can use as an admin:\n"
            "\n"
            "*Remember: With great power comes great responsibility!*\n"
            "\n" +
            formatColumns(commandNames.filter("-")) +
            "\n"
            "Type *help <command>* to see help about a particular command.\n"
            "Type *help admin-tips* to see some general tips for being an admin.";

    } else if (command == "admin-tips") {

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

    } else if (command == "triggers") {

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
        for (const QString &triggerName : m_triggers.keys()) {
            m += "  " + Util::highlight(triggerName) + "\n";
        }
        m += "\n"
             "Type *help <trigger>* to see help about a particular trigger.\n";

    } else {
        for (const QString &triggerName : m_triggers.keys()) {
            if (triggerName.startsWith(command)) {
                m = "\n" +
                    Util::highlight(triggerName) + "\n"
                    "  " + Util::splitLines(m_triggers[triggerName], 78).join("\n  ") +
                    "\n\n";
                break;
            }
        }
    }

    return m;
}

QString CommandInterpreter::formatColumns(const QStringList &commandNames) {

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
