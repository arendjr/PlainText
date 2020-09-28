mod command_executor;
mod command_interpreter;
mod command_line_processor;
mod command_registry;
mod go_command;
mod inventory_command;
mod look_command;

pub use command_executor::CommandExecutor;
use command_line_processor::CommandLineProcessor;

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum CommandType {
    Go,
    Inventory,
    Look,
    Lose,
}
