mod admin;
mod api;
mod close_command;
mod command_executor;
mod command_helpers;
mod command_interpreter;
mod command_line_processor;
mod command_registry;
mod follow_command;
mod go_command;
mod help_command;
mod inventory_command;
mod look_command;
mod lose_command;
mod open_command;

pub use command_executor::CommandExecutor;
use command_helpers::CommandHelpers;
use command_line_processor::CommandLineProcessor;

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum CommandType {
    ApiEntityCreate,
    ApiEntityDelete,
    ApiEntityList,
    ApiEntitySet,
    ApiPropertySet,
    Close(&'static str),
    EnterRoom(&'static str),
    Follow(&'static str),
    Go(&'static str),
    Help(&'static str),
    Inventory(&'static str),
    Look(&'static str),
    Lose(&'static str),
    Open(&'static str),
}

impl CommandType {
    pub fn description(&self) -> &'static str {
        use CommandType::*;
        match self {
            ApiEntityCreate => "",
            ApiEntityDelete => "",
            ApiEntityList => "",
            ApiEntitySet => "",
            ApiPropertySet => "",
            Close(description) => description,
            EnterRoom(description) => description,
            Follow(description) => description,
            Go(description) => description,
            Help(description) => description,
            Inventory(description) => description,
            Look(description) => description,
            Lose(description) => description,
            Open(description) => description,
        }
    }
}
