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
use super::command_interpreter::{interpret_command, InterpretationError};
use super::command_line_processor::CommandLineProcessor;
use super::command_registry::CommandRegistry;
use super::go_command::go;
use super::help_command::help;
use super::inventory_command::inventory;
use super::look_command::look;
use super::CommandType;

pub struct CommandExecutor {
    admin_command_registry: CommandRegistry,
    command_registry: CommandRegistry,
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
        match interpret_command(&self.command_registry, &mut processor).or_else(|err| {
            match realm.player(player_ref) {
                Some(player) if player.is_admin() => {
                    interpret_command(&self.admin_command_registry, &mut processor)
                }
                _ => Err(err),
            }
        }) {
            Ok(command_type) => {
                let command_helpers = CommandHelpers {
                    admin_command_registry: &self.admin_command_registry,
                    command_line_processor: &mut processor,
                    command_registry: &self.command_registry,
                    trigger_registry,
                };
                let command_fn = match command_type {
                    CommandType::ApiObjectCreate => wrap_api_request(object_create),
                    CommandType::ApiObjectDelete => wrap_api_request(object_delete),
                    CommandType::ApiObjectSet => wrap_api_request(object_set),
                    CommandType::ApiObjectsList => wrap_api_request(objects_list),
                    CommandType::ApiPropertySet => wrap_api_request(property_set),
                    CommandType::ApiTriggersList => wrap_api_request(triggers_list),
                    CommandType::EnterRoom(_) => wrap_admin_command(enter_room),
                    CommandType::Go(_) => wrap_command(go),
                    CommandType::Help(_) => wrap_command(help),
                    CommandType::Inventory(_) => wrap_command(inventory),
                    CommandType::Look(_) => wrap_command(look),
                    CommandType::Lose(_) => panic!("Not implemented"),
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
        let mut registry = CommandRegistry::new();
        registry.register("api-object-create", CommandType::ApiObjectCreate);
        registry.register("api-object-delete", CommandType::ApiObjectDelete);
        registry.register("api-object-set", CommandType::ApiObjectSet);
        registry.register("api-objects-list", CommandType::ApiObjectsList);
        registry.register("api-property-set", CommandType::ApiPropertySet);
        registry.register("api-triggers-list", CommandType::ApiTriggersList);
        registry.register(
            "enter",
            CommandType::Go(
                "Enter another room.\n\
                \n\
                Example: *enter door*",
            ),
        );
        registry.register(
            "examine",
            CommandType::Look(
                "Examine an object.\n\
                \n\
                Examples: *examine sign*, *examine key in inventory*",
            ),
        );
        registry.register(
            "go",
            CommandType::Go(
                "Go to another room.\n\
                \n\
                Examples: *go north*, *go to tower*, *go forward*",
            ),
        );
        registry.register(
            "help",
            CommandType::Help(
                "Shows in-game help, like the one you are now reading.\n\
                \n\
                Examples: *help*, *help buy*",
            ),
        );
        registry.register(
            "inventory",
            CommandType::Inventory(
                "Inspect your inventory.\n\
                \n\
                Examples: *inventory*, *look in inventory*.",
            ),
        );
        registry.register(
            "l",
            CommandType::Look(
                "Examine an object.\n\
                \n\
                Examples: *look north*, *look at sign*, *look at key in inventory*",
            ),
        );
        registry.register(
            "look",
            CommandType::Look(
                "Examine an object.\n\
                \n\
                Examples: *look north*, *look at sign*, *look at key in inventory*",
            ),
        );

        let admin_registry = {
            let mut registry = CommandRegistry::new();
            registry.register(
                "enter-room",
                CommandType::EnterRoom(
                    "Enter a room without the need for there to be an exit to the room.\n\
                    \n\
                    Example: *enter-room #1234*",
                ),
            );
            registry
        };

        Self {
            admin_command_registry: admin_registry,
            command_registry: registry,
        }
    }
}

pub fn wrap_command<F>(
    f: F,
) -> Box<dyn Fn(&mut Realm, GameObjectRef, CommandHelpers) -> Vec<PlayerOutput>>
where
    F: Fn(&mut Realm, GameObjectRef, CommandHelpers) -> Result<Vec<PlayerOutput>, String> + 'static,
{
    Box::new(
        move |realm, player_ref, helpers| match f(realm, player_ref, helpers) {
            Ok(output) => output,
            Err(mut message) => {
                message.push('\n');
                let mut output: Vec<PlayerOutput> = Vec::new();
                push_output_string!(output, player_ref, message);
                output
            }
        },
    )
}
