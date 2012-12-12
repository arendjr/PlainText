#include "helpcommand.h"

#include "commandregistry.h"
#include "realm.h"
#include "scriptengine.h"
#include "triggerregistry.h"
#include "util.h"


#define super Command

HelpCommand::HelpCommand(QObject *parent) :
    super(parent) {

    setDescription("Show in-game help, like the one you are now reading.\n"
                   "\n"
                   "Examples: help, help buy");
}

HelpCommand::~HelpCommand() {
}

void HelpCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    CommandRegistry *registry = realm()->commandRegistry();
    QString m;

    if (hasWordsLeft()) {
        QString commandName = takeWord();

        if (commandName == "commands") {
            QStringList commandNames = registry->commandNames().filter(QRegExp("^[^/]"));
            m = "\nHere is a list of all the commands you can use:\n\n" +
                Util::formatColumns(commandNames, Highlighted) +
                "\nType *help <command>* to see help about a particular command.";

            if (player->isAdmin()) {
                m += "\nTo see all the admin commands you can use, type *help admin-commands*.";
            }
        } else {
            if (registry->contains(commandName)) {
                m = "\n*" + commandName + "*\n  " +
                    Util::splitLines(registry->description(commandName), 77).join("\n  ") + "\n\n";
            } else if (player->isAdmin()) {
                if (registry->adminCommandsContains(commandName)) {
                    m = "\n*" + commandName + "*\n  " +
                        Util::splitLines(registry->adminCommandDescription(commandName),
                                         77).join("\n  ") + "\n\n";
                } else if (registry->apiCommandsContains(commandName)) {
                    m = "\n*" + commandName + "*\n  " +
                        Util::splitLines(registry->apiCommandDescription(commandName),
                                         77).join("\n  ") + "\n\n";
                } else {
                    m = showAdminHelp(commandName);
                }
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

    CommandRegistry *registry = realm()->commandRegistry();
    QString m;

    if (commandName == "admin-commands") {
        m = "\n"
            "Here is a list of all the commands you can use as an admin:\n"
            "\n"
            "*Remember: With great power comes great responsibility!*\n"
            "\n" +
            Util::formatColumns(registry->adminCommandNames(), Highlighted) +
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
            "Similarly, you can always use the word *room* to refer to the current room you are "
            "in. But, to make editing of rooms even easier, you can use *@<property>* as a "
            "shortcut for *get-* or *set-prop room <property>*. Thus, you can simply type: "
            "*@id* to get the ID of the current room. Or, to set the description: *@description As "
            "you stand just outside the South Gate, ...*\n"
            "\n"
            "Not listed in the admin commands overview, but very useful: *edit-prop* and "
            "*edit-trigger* are available for more convenient editing of properties and triggers. "
            "The usage is the same as for *get-prop* and *get-trigger*. Note that these commands "
            "are only available if you use the web interface (not supported when using telnet).\n"
            "\n";
    } else if (commandName == "triggers") {
        m = "\n"
            "*Trigger Overview*\n"
            "\n"
            "Here is a list of all the triggers which are available:\n"
            "\n";
        for (const QString &triggerName : realm()->triggerRegistry()->signatures()) {
            m += "  " + Util::highlight(triggerName) + "\n";
        }
        m += "\n"
             "Type *help <trigger>* to see help about a particular trigger.\n";
    } else {
        for (const QString &triggerName : realm()->triggerRegistry()->signatures()) {
            if (triggerName.startsWith(commandName)) {
                m = "\n" +
                    Util::highlight(triggerName) + "\n  " +
                    Util::splitLines(realm()->triggerRegistry()->description(triggerName), 77)
                    .join("\n  ") +
                    "\n\n";
                break;
            }
        }
    }

    return m;
}
