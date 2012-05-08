#include "commandinterpreter.h"

#include <QRegExp>
#include <QStringList>

#include "area.h"
#include "character.h"
#include "exit.h"
#include "gameobjectptr.h"
#include "realm.h"
#include "util.h"
#include "commands/gocommand.h"
#include "commands/quitcommand.h"
#include "commands/saycommand.h"
#include "commands/slashmecommand.h"
#include "commands/admin/addexitcommand.h"
#include "commands/admin/getpropcommand.h"
#include "commands/admin/removeexitcommand.h"
#include "commands/admin/setpropcommand.h"
#include "commands/admin/stopservercommand.h"


CommandInterpreter::CommandInterpreter(Character *character, QObject *parent) :
    QObject(parent),
    m_character(character) {

    Command *go = new GoCommand(character, this);
    Command *quit = new QuitCommand(character, this);
    Command *say = new SayCommand(character, this);
    Command *slashMe = new SlashMeCommand(character, this);

    m_commands.insert("go", go);
    m_commands.insert("goodbye", quit);
    m_commands.insert("quit", quit);
    m_commands.insert("say", say);
    m_commands.insert("/me", slashMe);

    if (m_character->isAdmin()) {
        m_commands.insert("get-prop", new GetPropCommand(character, this));
        m_commands.insert("set-prop", new SetPropCommand(character, this));

        m_commands.insert("add-exit", new AddExitCommand(character, this));
        m_commands.insert("remove-exit", new RemoveExitCommand(character, this));

        m_commands.insert("stop-server", new StopServerCommand(character, this));
    }

    connect(quit, SIGNAL(quit()), this, SIGNAL(quit()));
}

CommandInterpreter::~CommandInterpreter() {
}

void CommandInterpreter::execute(const QString &command) {

    QStringList words = command.trimmed().split(QRegExp("\\s+"));
    words[0] = words[0].toLower();

    if (Util::isDirectionAbbreviation(words[0])) {
        words[0] = Util::direction(words[0]);
    }
    if (Util::isDirection(words[0])) {
        words.prepend("go");

        if (m_commands.contains(words[0])) {
            m_commands[words[0]]->execute(words.join(" "));
            return;
        }
    }

    if (m_commands.contains(words[0])) {
        m_commands[words[0]]->execute(command);
        return;
    }

    m_character->send(QString("The command \"%1\" does not exist.").arg(words[0]));
}
