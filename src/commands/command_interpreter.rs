use crate::direction_utils::{direction_by_abbreviation, is_direction};

use super::command_registry::{CommandRegistry, LookupError};
use super::{CommandLineProcessor, CommandType};

pub enum InterpretationError {
    NoCommand,
    AmbiguousCommand(String),
    UnknownCommand(String),
}

pub struct CommandInterpreter {
    registry: CommandRegistry,
}

impl CommandInterpreter {
    pub fn interpret_command(
        &self,
        processor: &mut CommandLineProcessor,
    ) -> Result<CommandType, InterpretationError> {
        let mut command_name = processor
            .peek_word()
            .ok_or(InterpretationError::NoCommand)?;

        if let Some(direction) = direction_by_abbreviation(command_name) {
            command_name = direction;
        }

        if is_direction(command_name) {
            processor.prepend_word("go".to_owned());
            return Ok(CommandType::Go);
        }

        match self.registry.lookup(command_name) {
            Ok(command_type) => Ok(command_type),
            Err(LookupError::NotFound) => {
                Err(InterpretationError::UnknownCommand(command_name.to_owned()))
            }
            Err(LookupError::NotUnique) => Err(InterpretationError::AmbiguousCommand(
                command_name.to_owned(),
            )),
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
    }

    pub fn new() -> Self {
        let mut registry = CommandRegistry::new();
        registry.register("api-objects-list", CommandType::ApiObjectsList);
        registry.register("api-property-set", CommandType::ApiPropertySet);
        registry.register("api-triggers-list", CommandType::ApiTriggersList);
        registry.register("enter", CommandType::Go);
        registry.register("examine", CommandType::Look);
        registry.register("go", CommandType::Go);
        registry.register("inventory", CommandType::Inventory);
        registry.register("l", CommandType::Look);
        registry.register("look", CommandType::Look);

        Self { registry }
    }
}
