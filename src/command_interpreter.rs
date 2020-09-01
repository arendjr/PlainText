use std::collections::VecDeque;
use std::iter::Iterator;

use crate::commands::Command;
use crate::direction_utils::{get_direction_by_abbreviation, is_direction};

pub enum InterpretationError {
    NoCommand,
    AmbiguousCommand(String),
    UnknownCommand(String),
}

pub fn interpret_command(command: String) -> Result<Command, InterpretationError> {
    let mut words = command
        .split(' ')
        .map(|word| word.trim())
        .filter_map(|word| if word.len() > 0 { Some(word) } else { None })
        .collect::<VecDeque<&str>>();

    let mut command_name = match words.pop_front() {
        Some(word) => word.to_lowercase(),
        None => return Err(InterpretationError::NoCommand),
    };

    if let Some(direction) = get_direction_by_abbreviation(&command_name) {
        command_name = direction.to_owned();
    }

    if is_direction(&command_name) {
        return Ok(Command::Go(
            words
                .into_iter()
                .fold(String::new(), |acc, word| acc + " " + word),
        ));
    }

    // TODO: Implement the rest

    Err(InterpretationError::UnknownCommand(command_name))
}

/*
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
*/
