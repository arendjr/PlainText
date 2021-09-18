mod admin;
mod api;
mod command_executor;
mod command_helpers;
mod command_interpreter;
mod command_line_processor;
mod command_registry;
mod go_command;
mod help_command;
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
    EnterRoom(&'static str),
    Go(&'static str),
    Help(&'static str),
    Inventory(&'static str),
    Look(&'static str),
    Lose(&'static str),
}

impl CommandType {
    pub fn description(&self) -> &'static str {
        use CommandType::*;
        match self {
            ApiObjectCreate => "",
            ApiObjectDelete => "",
            ApiObjectSet => "",
            ApiObjectsList => "",
            ApiPropertySet => "",
            ApiTriggersList => "",
            EnterRoom(description) => description,
            Go(description) => description,
            Help(description) => description,
            Inventory(description) => description,
            Look(description) => description,
            Lose(description) => description,
        }
    }
}
