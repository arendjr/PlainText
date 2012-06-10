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
#include "commands/closecommand.h"
#include "commands/dropcommand.h"
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
#include "commands/whocommand.h"
#include "commands/admin/addcharactercommand.h"
#include "commands/admin/addexitcommand.h"
#include "commands/admin/additemcommand.h"
#include "commands/admin/execscriptcommand.h"
#include "commands/admin/getpropcommand.h"
#include "commands/admin/gettriggercommand.h"
#include "commands/admin/removeexitcommand.h"
#include "commands/admin/removeitemcommand.h"
#include "commands/admin/setpropcommand.h"
#include "commands/admin/settriggercommand.h"
#include "commands/admin/stopservercommand.h"
#include "commands/admin/unsettriggercommand.h"


CommandInterpreter::CommandInterpreter(Player *player, QObject *parent) :
    QObject(parent),
    m_player(player) {

    Command *close = new CloseCommand(player, this);
    Command *drop = new DropCommand(player, this);
    Command *get = new GetCommand(player, this);
    Command *give = new GiveCommand(player, this);
    Command *go = new GoCommand(player, this);
    Command *inventory = new InventoryCommand(player, this);
    Command *kill = new KillCommand(player, this);
    Command *look = new LookCommand(player, this);
    Command *open = new OpenCommand(player, this);
    Command *quit = new QuitCommand(player, this);
    Command *say = new SayCommand(player, this);
    Command *shout = new ShoutCommand(player, this);
    Command *slashMe = new SlashMeCommand(player, this);
    Command *stats = new StatsCommand(player, this);
    Command *talk = new TalkCommand(player, this);
    Command *tell = new TellCommand(player, this);
    Command *who = new WhoCommand(player, this);

    m_commands.insert("attack", kill);
    m_commands.insert("close", close);
    m_commands.insert("drop", drop);
    m_commands.insert("enter", go);
    m_commands.insert("give", give);
    m_commands.insert("get", get);
    m_commands.insert("go", go);
    m_commands.insert("goodbye", quit);
    m_commands.insert("inventory", inventory);
    m_commands.insert("kill", kill);
    m_commands.insert("look", look);
    m_commands.insert("open", open);
    m_commands.insert("quit", quit);
    m_commands.insert("say", say);
    m_commands.insert("shout", shout);
    m_commands.insert("stats", stats);
    m_commands.insert("take", get);
    m_commands.insert("talk", talk);
    m_commands.insert("tell", tell);
    m_commands.insert("who", who);
    m_commands.insert("/me", slashMe);

    if (m_player->isAdmin()) {
        m_commands.insert("get-prop", new GetPropCommand(player, this));
        m_commands.insert("set-prop", new SetPropCommand(player, this));

        m_commands.insert("get-trigger", new GetTriggerCommand(player, this));
        m_commands.insert("set-trigger", new SetTriggerCommand(player, this));
        m_commands.insert("unset-trigger", new UnsetTriggerCommand(player, this));

        m_commands.insert("add-item", new AddItemCommand(player, this));
        m_commands.insert("remove-item", new RemoveItemCommand(player, this));

        m_commands.insert("add-exit", new AddExitCommand(player, this));
        m_commands.insert("remove-exit", new RemoveExitCommand(player, this));

        m_commands.insert("add-character", new AddCharacterCommand(player, this));

        m_commands.insert("exec-script", new ExecScriptCommand(player, this));

        m_commands.insert("stop-server", new StopServerCommand(player, this));


        m_triggers.insert("onactive : void",
                          "The onactive trigger is invoked on any character when it's no longer "
                          "stunned, ie. when it can perform a new action again.");
        m_triggers.insert("onattack(attacker : character) : bool",
                          "The onattack trigger is invoked on any character when it's being "
                          "attacked.");
        m_triggers.insert("oncharacterattacked(attacker : character, defendant : character) : void",
                          "The oncharacterattacked trigger is invoked on any character in an area, "
                          "except for the attacker and defendant themselves, when another character "
                          "in that area emerges into combat.");
        m_triggers.insert("oncharacterdied(defendant : character, attacker : optional character) : bool",
                          "The oncharacterdied trigger is invoked on any character in an area, when "
                          "another character in that area dies.");
        m_triggers.insert("oncharacterentered(activator : character) : void",
                          "The oncharacterentered trigger is invoked on any character in an area "
                          "when another character enters that area.");
        m_triggers.insert("onclose(activator : character) : bool",
                          "The onclose trigger is invoked on any item or exit when it's closed.");
        m_triggers.insert("ondie(attacker : optional character) : bool",
                          "The ondie trigger is invoked on any character when it dies.");
        m_triggers.insert("onenter(activator : character) : bool",
                          "The onenter trigger is invoked on any exit when it's entered.");
        m_triggers.insert("onexit(activator : character, exitName : string) : bool",
                          "The onexit trigger is invoked on any character in an area when another "
                          "character leaves that area.");
        m_triggers.insert("oninit : void",
                          "The oninit trigger is invoked once on every object when the game server "
                          "is started.");
        m_triggers.insert("onopen(activator : character) : bool",
                          "The onopen trigger is invoked on any item or exit when it's opened.");
        m_triggers.insert("onspawn : void",
                          "The onspawn trigger is invoked on any character when it respawns.");
        m_triggers.insert("ontalk(speaker : character, message : string) : void",
                          "The ontalk trigger is invoked on any character when talked to.");
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
        foreach (const QString &key, m_commands.keys()) {
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
    } catch (const GameException &exception) {
        m_player->send(QString("Executing the command gave an exception: %1").arg(exception.what()));
        if (m_player->isAdmin()) {
            m_player->send("This is not good. You may want to contact a game admin about this.");
        }
    }
}

void CommandInterpreter::showHelp(const QString &command) {

    if (command.isEmpty()) {
        m_player->send(QString("Type %1 to see a list of all commands.\n"
                               "Type %2 to see help about a particular command.")
                       .arg(Util::highlight("help commands"),
                            Util::highlight("help <command>")));
    } else if (command == "commands") {
        m_player->send("Here is a list of all the commands you can use:\n\n");

        QStringList commandNames = m_commands.keys();
        showColumns(commandNames.filter(QRegExp("^\\w+$")));

        m_player->send(QString("\nType %1 to see help about a particular command.")
                       .arg(Util::highlight("help <command>")));

        if (m_player->isAdmin()) {
            m_player->send(QString("\nTo see all the admin commands you can use, type %1.")
                           .arg(Util::highlight("help admin-commands")));
        }
    } else if (command == "admin-commands") {
        if (m_player->isAdmin()) {
            m_player->send("Here is a list of all the commands you can use as an admin:\n\n" +
                           Util::highlight("Remember: With great power comes great responsibility!") + "\n\n");

            QStringList commandNames = m_commands.keys();
            showColumns(commandNames.filter("-"));

            m_player->send(QString("\nType %1 to see help about a particular command.")
                           .arg(Util::highlight("help <command>")));
        } else {
            m_player->send("Sorry, but you don't look much like an admin to me.");
        }
    } else if (command == "triggers") {
        if (m_player->isAdmin()) {
            m_player->send("Here is a list of all the triggers which are available:\n\n");

            foreach (const QString &triggerName, m_triggers.keys()) {
                m_player->send("  " + Util::highlight(triggerName));
            }

            m_player->send(QString("\nType %1 to see help about a particular trigger.\n"
                                   "Note: For any trigger with a boolean return type, returning "
                                   "false will have the effect of canceling the original action.")
                           .arg(Util::highlight("help <trigger>")));
        } else {
            m_player->send("Sorry, but you don't look much like an admin to me.");
        }
    } else {
        if (m_commands.contains(command)) {
            m_player->send("\n" + Util::highlight(command) + "\n"
                           "  " + Util::splitLines(m_commands[command]->description(), 78).join("\n  ") + "\n\n");
            return;
        }

        if (m_player->isAdmin()) {
            foreach (const QString &triggerName, m_triggers.keys()) {
                if (triggerName.startsWith(command)) {
                    m_player->send("\n" + Util::highlight(triggerName) + "\n"
                                   "  " + Util::splitLines(m_triggers[triggerName], 78).join("\n  ") + "\n\n");
                    return;
                }
            }
        }

        m_player->send(QString("The command %1 is not recognized.\n"
                               "Type %2 to see a list of all commands.")
                       .arg(Util::highlight(command),
                            Util::highlight("help commands")));
    }
}

void CommandInterpreter::showColumns(const QStringList &commandNames) {

    int length = commandNames.length();
    int halfLength = length / 2 + length % 2;
    for (int i = 0; i < halfLength; i++) {
        QString first = commandNames[i];
        QString second = i + halfLength < length ? commandNames[i + halfLength] : "";

        m_player->send("  " + Util::highlight(first.leftJustified(30)) +
                       "  " + Util::highlight(second));
    }
}
