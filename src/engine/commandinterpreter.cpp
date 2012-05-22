#include "commandinterpreter.h"

#include <QRegExp>
#include <QStringList>

#include "area.h"
#include "exit.h"
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
#include "commands/lookcommand.h"
#include "commands/opencommand.h"
#include "commands/quitcommand.h"
#include "commands/saycommand.h"
#include "commands/shoutcommand.h"
#include "commands/slashmecommand.h"
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
    Command *look = new LookCommand(player, this);
    Command *open = new OpenCommand(player, this);
    Command *quit = new QuitCommand(player, this);
    Command *say = new SayCommand(player, this);
    Command *shout = new ShoutCommand(player, this);
    Command *slashMe = new SlashMeCommand(player, this);
    Command *talk = new TalkCommand(player, this);
    Command *tell = new TellCommand(player, this);
    Command *who = new WhoCommand(player, this);

    m_commands.insert("close", close);
    m_commands.insert("drop", drop);
    m_commands.insert("enter", go);
    m_commands.insert("give", give);
    m_commands.insert("get", get);
    m_commands.insert("go", go);
    m_commands.insert("goodbye", quit);
    m_commands.insert("inventory", inventory);
    m_commands.insert("look", look);
    m_commands.insert("open", open);
    m_commands.insert("quit", quit);
    m_commands.insert("say", say);
    m_commands.insert("shout", shout);
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
    }

    connect(quit, SIGNAL(quit()), this, SIGNAL(quit()));
}

CommandInterpreter::~CommandInterpreter() {
}

void CommandInterpreter::execute(const QString &command) {

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
        commandName = Util::direction(commandName);
    }
    if (Util::isDirection(commandName)) {
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
}

void CommandInterpreter::showHelp(const QString &command) {

    if (command.isEmpty()) {
        m_player->send(QString("Type %1 to see a list of all commands.\n"
                               "Type %2 to see help about a particular command.")
                       .arg(Util::colorize("help commands", White),
                            Util::colorize("help <command>", White)));
    } else if (command == "commands") {
        m_player->send("Here is a list of all the commands you can use:\n\n");

        QStringList commandNames = m_commands.keys();
        showColumns(commandNames.filter(QRegExp("^\\w+$")));

        if (m_player->isAdmin()) {
            m_player->send(QString("\nTo see all the admin commands you can use, type %1.")
                           .arg(Util::colorize("help admin-commands", White)));
        }
    } else if (command == "admin-commands") {
        if (m_player->isAdmin()) {
            m_player->send("Here is a list of all the commands you can use as an admin:\n\n" +
                           Util::colorize("Remember: With great power comes great responsibility!", White) + "\n\n");

            QStringList commandNames = m_commands.keys();
            showColumns(commandNames.filter("-"));
        } else {
            m_player->send("Sorry, but you don't look much like an admin to me.");
        }
    } else {
        if (m_commands.contains(command)) {
            m_player->send("\n" + Util::colorize(command, White) + "\n"
                           "  " + Util::splitLines(m_commands[command]->description(), 78).join("\n  ") + "\n\n");
        } else {
            m_player->send(QString("The command %1 is not recognized.\n"
                                   "Type %2 to see a list of all commands.")
                           .arg(Util::colorize(command, White),
                                Util::colorize("help commands", White)));
        }
    }
}

void CommandInterpreter::showColumns(const QStringList &commandNames) {

    int length = commandNames.length();
    int halfLength = length / 2 + length % 2;
    for (int i = 0; i < halfLength; i++) {
        QString first = commandNames[i];
        QString second = i + halfLength < length ? commandNames[i + halfLength] : "";

        m_player->send("  " + Util::colorize(first.leftJustified(30), White) +
                       "  " + Util::colorize(second, White));
    }
}
