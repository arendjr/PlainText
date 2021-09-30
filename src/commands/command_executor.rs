use super::{
    admin::enter_room,
    api::{objects_list, property_set},
    close_command::close,
    command_helpers::CommandHelpers,
    command_interpreter::{interpret_command, InterpretationError},
    command_line_processor::CommandLineProcessor,
    command_registry::CommandRegistry,
    follow_command::follow,
    go_command::go,
    help_command::help,
    inventory_command::inventory,
    look_command::look,
    lose_command::lose,
    open_command::open,
    CommandType,
};
use crate::{
    commands::admin::wrap_admin_command,
    commands::api::{object_create, object_delete, object_set, wrap_api_request},
    game_object::GameObjectRef,
    objects::Realm,
    player_output::PlayerOutput,
    LogSender,
};

pub struct CommandExecutor {
    admin_command_registry: CommandRegistry,
    command_registry: CommandRegistry,
}

impl CommandExecutor {
    pub fn execute_command(
        &self,
        realm: &mut Realm,
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
                };
                let command_fn = match command_type {
                    CommandType::ApiObjectCreate => wrap_api_request(object_create),
                    CommandType::ApiObjectDelete => wrap_api_request(object_delete),
                    CommandType::ApiObjectSet => wrap_api_request(object_set),
                    CommandType::ApiObjectsList => wrap_api_request(objects_list),
                    CommandType::ApiPropertySet => wrap_api_request(property_set),
                    CommandType::Close(_) => wrap_command(close),
                    CommandType::EnterRoom(_) => wrap_admin_command(enter_room),
                    CommandType::Follow(_) => wrap_command(follow),
                    CommandType::Go(_) => wrap_command(go),
                    CommandType::Help(_) => wrap_command(help),
                    CommandType::Inventory(_) => wrap_command(inventory),
                    CommandType::Look(_) => wrap_command(look),
                    CommandType::Lose(_) => wrap_command(lose),
                    CommandType::Open(_) => wrap_command(open),
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
        registry.register(
            "close",
            CommandType::Close(
                "Close an exit, typically a door or a window.\n\
                \n\
                Example: *close door*",
            ),
        );
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
            "follow",
            CommandType::Follow(
                "Form or join a group by following another player. The first person \
                being followed automatically becomes the group leader.\n\
                \n\
                Example: *follow mia*",
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
        registry.register(
            "lose",
            CommandType::Lose(
                "Remove yourself or someone else from a group. If you are a group \
                leader, you can remove anyone from your group by using *lose <name>*. \
                You can always remove yourself from a group using simply *lose*.\n\
                \n\
                Examples: *lose mia*, *lose*",
            ),
        );
        registry.register(
            "open",
            CommandType::Open(
                "Open an exit, typically a door or a window. Note that doors may \
                automatically close after a while.\n\
                \n\
                Example: *open door*",
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
