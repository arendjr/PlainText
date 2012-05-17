#include "commandinterpreter.h"

#include <QRegExp>
#include <QStringList>

#include "area.h"
#include "character.h"
#include "exit.h"
#include "gameobjectptr.h"
#include "realm.h"
#include "util.h"
#include "commands/dropcommand.h"
#include "commands/getcommand.h"
#include "commands/givecommand.h"
#include "commands/gocommand.h"
#include "commands/lookcommand.h"
#include "commands/quitcommand.h"
#include "commands/saycommand.h"
#include "commands/slashmecommand.h"
#include "commands/admin/addexitcommand.h"
#include "commands/admin/additemcommand.h"
#include "commands/admin/execscriptcommand.h"
#include "commands/admin/getpropcommand.h"
#include "commands/admin/removeexitcommand.h"
#include "commands/admin/removeitemcommand.h"
#include "commands/admin/setpropcommand.h"
#include "commands/admin/stopservercommand.h"


CommandInterpreter::CommandInterpreter(Character *character, QObject *parent) :
    QObject(parent),
    m_character(character) {

    Command *drop = new DropCommand(character, this);
    Command *get = new GetCommand(character, this);
    Command *give = new GiveCommand(character, this);
    Command *go = new GoCommand(character, this);
    Command *look = new LookCommand(character, this);
    Command *quit = new QuitCommand(character, this);
    Command *say = new SayCommand(character, this);
    Command *slashMe = new SlashMeCommand(character, this);

    m_commands.insert("drop", drop);
    m_commands.insert("look", look);
    m_commands.insert("give", give);
    m_commands.insert("get", get);
    m_commands.insert("go", go);
    m_commands.insert("goodbye", quit);
    m_commands.insert("quit", quit);
    m_commands.insert("say", say);
    m_commands.insert("take", get);
    m_commands.insert("/me", slashMe);

    if (m_character->isAdmin()) {
        m_commands.insert("get-prop", new GetPropCommand(character, this));
        m_commands.insert("set-prop", new SetPropCommand(character, this));

        m_commands.insert("add-item", new AddItemCommand(character, this));
        m_commands.insert("remove-item", new RemoveItemCommand(character, this));

        m_commands.insert("add-exit", new AddExitCommand(character, this));
        m_commands.insert("remove-exit", new RemoveExitCommand(character, this));

        m_commands.insert("exec-script", new ExecScriptCommand(character, this));

        m_commands.insert("stop-server", new StopServerCommand(character, this));
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
        m_character->send("Command is not unique.\n");
    } else {
        m_character->send(QString("Command \"%1\" does not exist.\n").arg(words[0]));
    }
}
