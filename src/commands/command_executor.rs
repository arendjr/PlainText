use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::LogSender;

use super::command_interpreter::{CommandInterpreter, InterpretationError};
use super::command_line_processor::CommandLineProcessor;
use super::go_command::go;
use super::inventory_command::inventory;
use super::look_command::look;
use super::CommandType;

pub struct CommandExecutor {
    interpreter: CommandInterpreter,
}

impl CommandExecutor {
    pub fn execute_command(
        &self,
        realm: Realm,
        player_ref: GameObjectRef,
        _: &LogSender,
        command_line: String,
    ) -> (Realm, Vec<PlayerOutput>) {
        let mut processor = CommandLineProcessor::new(player_ref, &command_line);
        match self.interpreter.interpret_command(&mut processor) {
            Ok(command_type) => {
                let command_fn = match command_type {
                    CommandType::Go => go,
                    CommandType::Inventory => inventory,
                    CommandType::Look => look,
                    CommandType::Lose => panic!("Not implemented"),
                };
                command_fn(realm, player_ref, processor)
            }
            Err(InterpretationError::AmbiguousCommand(_)) => (
                realm,
                vec![PlayerOutput::new_from_str(
                    player_ref.id(),
                    "Command is not unique.\n",
                )],
            ),
            Err(InterpretationError::UnknownCommand(command)) => (
                realm,
                vec![PlayerOutput::new_from_string(
                    player_ref.id(),
                    format!("Command \"{}\" does not exist.\n", command),
                )],
            ),
            Err(InterpretationError::NoCommand) => (realm, vec![]),
        }
    }

    pub fn new() -> Self {
        Self {
            interpreter: CommandInterpreter::new(),
        }
    }
}
