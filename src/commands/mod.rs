mod api;
mod command_executor;
mod command_helpers;
mod command_interpreter;
mod command_line_processor;
mod command_registry;
mod go_command;
mod inventory_command;
mod look_command;

pub use command_executor::CommandExecutor;
use command_helpers::CommandHelpers;
use command_line_processor::CommandLineProcessor;

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum CommandType {
    ApiObjectCreate,
    ApiObjectDelete,
    ApiObjectSet,
    ApiObjectsList,
    ApiPropertySet,
    ApiTriggersList,
    Go,
    Inventory,
    Look,
    Lose,
}
