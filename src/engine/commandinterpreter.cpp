#include "commandinterpreter.h"

#include <QRegExp>
#include <QStringList>

#include "command.h"
#include "commandregistry.h"
#include "exit.h"
#include "gameexception.h"
#include "gameobjectptr.h"
#include "logutil.h"
#include "player.h"
#include "room.h"
#include "util.h"


CommandInterpreter::CommandInterpreter(QObject *parent) :
    QObject(parent),
    m_registry(nullptr) {
}

CommandInterpreter::~CommandInterpreter() {
}

void CommandInterpreter::setRegistry(CommandRegistry *registry) {

    m_registry = registry;
}

void CommandInterpreter::execute(Player *player, const QString &command) {

    static QRegExp whitespace("\\s+");

    try {
        LogUtil::logCommand(player->name(), command);

        QStringList words = command.trimmed().split(whitespace);
        QString commandName = words[0].toLower();
        if (commandName.isEmpty()) {
            return;
        }

        if (Util::isDirectionAbbreviation(commandName)) {
            words[0] = Util::direction(commandName);
            commandName = words[0];
        }
        Room *currentRoom = player->currentRoom().cast<Room *>();
        bool matchedExit = false;
        for (const GameObjectPtr &exitPtr : currentRoom->exits()) {
            if (exitPtr->name() == commandName) {
                matchedExit = true;
            }
        }
        if (Util::isDirection(commandName) || matchedExit) {
            words.prepend("go");
            m_registry->command("go")->execute(player, words.join(" "));
            return;
        }

        QStringList commands;

        if (player->isAdmin()) {
            if (commandName.startsWith("api-")) {
                if (m_registry->apiCommandsContains(commandName)) {
                    m_registry->apiCommand(commandName)->execute(player, command);
                    return;
                }
            } else {
                if (m_registry->adminCommandsContains(commandName)) {
                    m_registry->adminCommand(commandName)->execute(player, command);
                    return;
                } else {
                    for (const QString &name : m_registry->adminCommandNames()) {
                        if (name.startsWith(commandName)) {
                            commands << name;
                        }
                    }
                }
            }
        }

        if (m_registry->contains(commandName)) {
            m_registry->command(commandName)->execute(player, command);
            return;
        } else {
            for (const QString &name : m_registry->commandNames()) {
                if (name.startsWith(commandName)) {
                    commands << name;
                }
            }
        }

        if (commands.length() == 1) {
            commandName = commands[0];
            if (m_registry->contains(commandName)) {
                m_registry->command(commandName)->execute(player, command);
            } else if (m_registry->adminCommandsContains(commandName)) {
                m_registry->adminCommand(commandName)->execute(player, command);
            }
        } else if (commands.length() > 1) {
            player->send("Command is not unique.");
        } else {
            player->send(QString("Command \"%1\" does not exist.").arg(words[0]));
        }
    } catch (GameException &exception) {
        player->send(QString("Executing the command gave an exception: %1").arg(exception.what()));
        if (!player->isAdmin()) {
            player->send("This is not good. You may want to contact a game admin about this.");
        }
    } catch (...) {
        player->send("Executing the command gave an unknown exception.");
        if (!player->isAdmin()) {
            player->send("This is not good. You may want to contact a game admin about this.");
        }
    }
}
