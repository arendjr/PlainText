use crate::commands::admin::wrap_admin_command;
use crate::commands::api::{object_create, object_delete, object_set, wrap_api_request};
use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::trigger_registry::TriggerRegistry;
use crate::LogSender;

use super::admin::enter_room;
use super::api::{objects_list, property_set, triggers_list};
use super::command_helpers::CommandHelpers;
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
        realm: &mut Realm,
        trigger_registry: &TriggerRegistry,
        player_ref: GameObjectRef,
        _: &LogSender,
        command_line: String,
    ) -> Vec<PlayerOutput> {
        let mut processor = CommandLineProcessor::new(player_ref, &command_line);
        match self.interpreter.interpret_command(&mut processor) {
            Ok(command_type) => {
                let command_helpers = CommandHelpers {
                    command_line_processor: &mut processor,
                    trigger_registry,
                };
                let command_fn = match command_type {
                    CommandType::ApiObjectCreate => wrap_api_request(object_create),
                    CommandType::ApiObjectDelete => wrap_api_request(object_delete),
                    CommandType::ApiObjectSet => wrap_api_request(object_set),
                    CommandType::ApiObjectsList => wrap_api_request(objects_list),
                    CommandType::ApiPropertySet => wrap_api_request(property_set),
                    CommandType::ApiTriggersList => wrap_api_request(triggers_list),
                    CommandType::EnterRoom => wrap_admin_command(enter_room),
                    CommandType::Go => wrap_command(go),
                    CommandType::Inventory => wrap_command(inventory),
                    CommandType::Look => wrap_command(look),
                    CommandType::Lose => panic!("Not implemented"),
                };
                command_fn(realm, player_ref, command_helpers)
            }
            Err(InterpretationError::AmbiguousCommand(_)) => vec![PlayerOutput::new_from_str(
                player_ref.id(),
                "Command is not unique.\n",
            )],
            Err(InterpretationError::UnknownCommand(command)) => {
                vec![PlayerOutput::new_from_string(
                    player_ref.id(),
                    format!("Command \"{}\" does not exist.\n", command),
                )]
            }
            Err(InterpretationError::NoCommand) => vec![],
        }
    }

    pub fn new() -> Self {
        Self {
            interpreter: CommandInterpreter::new(),
        }
    }
}

pub fn wrap_command<F>(
    f: F,
) -> Box<dyn Fn(&mut Realm, GameObjectRef, CommandHelpers) -> Vec<PlayerOutput>>
where
    F: Fn(&mut Realm, GameObjectRef, CommandHelpers) -> Vec<PlayerOutput> + 'static,
{
    Box::new(f)
}
