#include "commandinterpreter.h"

#include <QRegExp>
#include <QStringList>

#include "command.h"
#include "commandregistry.h"
#include "gameexception.h"
#include "gameobjectptr.h"
#include "logutil.h"
#include "player.h"
#include "portal.h"
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

void CommandInterpreter::execute(Character *character, const QString &_command) {

    static QRegExp whitespace("\\s+");

    try {
        QString command = _command.trimmed();
        LogUtil::logCommand(character->name(), command);

        QStringList words = command.split(whitespace);
        QString commandName = words[0].toLower();
        if (commandName.isEmpty()) {
            return;
        }

        if (Util::isDirectionAbbreviation(commandName)) {
            words[0] = Util::direction(commandName);
            commandName = words[0];
        }
        if (Util::isDirection(commandName)) {
            words.prepend("go");
            m_registry->command("go")->execute(character, words.join(" "));
            return;
        } else {
            Room *currentRoom = character->currentRoom().cast<Room *>();
            bool matchedPortal = false;
            for (const GameObjectPtr &portalPtr : currentRoom->portals()) {
                Portal *portal = portalPtr.cast<Portal *>();
                QString portalName = portal->nameFromRoom(character->currentRoom());
                if (portalName == commandName) {
                    matchedPortal = true;
                } else {
                    for (const QString &portalNamePart : portalName.split(' ')) {
                        if (portalNamePart == commandName) {
                            matchedPortal = true;
                        }
                    }
                }
            }
            if (matchedPortal) {
                words.prepend("go");
                m_registry->command("go")->execute(character, words.join(" "));
                return;
            }
        }

        QStringList commands;

        if (m_registry->contains(commandName)) {
            m_registry->command(commandName)->execute(character, command);
            return;
        } else {
            for (const QString &name : m_registry->commandNames()) {
                if (name.startsWith(commandName)) {
                    commands.append(name);
                }
            }
        }

        if (character->isPlayer() && qobject_cast<Player *>(character)->isAdmin()) {
            if (commandName.startsWith("api-")) {
                if (m_registry->apiCommandsContains(commandName)) {
                    m_registry->apiCommand(commandName)->execute(character, command);
                    return;
                }
            } else {
                if (m_registry->adminCommandsContains(commandName)) {
                    m_registry->adminCommand(commandName)->execute(character, command);
                    return;
                } else {
                    for (const QString &name : m_registry->adminCommandNames()) {
                        if (name.startsWith(commandName)) {
                            commands.append(name);
                        }
                    }
                }
            }
        }

        if (commands.length() == 1) {
            commandName = commands[0];
            if (m_registry->contains(commandName)) {
                m_registry->command(commandName)->execute(character, command);
            } else if (m_registry->adminCommandsContains(commandName)) {
                m_registry->adminCommand(commandName)->execute(character, command);
            }
        } else if (commands.length() > 1) {
            character->send("Command is not unique.");
        } else {
            character->send(QString("Command \"%1\" does not exist.").arg(words[0]));
        }
    } catch (GameException &exception) {
        if (character->isPlayer()) {
            Player *player = qobject_cast<Player *>(character);
            player->send(QString("Executing the command gave an exception: ") + exception.what());
            if (!player->isAdmin()) {
                player->send("This is not good. You may want to contact a game admin about this.");
            }
        }
    } catch (...) {
        if (character->isPlayer()) {
            Player *player = qobject_cast<Player *>(character);
            player->send("Executing the command gave an unknown exception.");
            if (!player->isAdmin()) {
                player->send("This is not good. You may want to contact a game admin about this.");
            }
        }
    }
}
