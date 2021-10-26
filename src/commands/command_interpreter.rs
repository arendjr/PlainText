use super::command_registry::{CommandRegistry, LookupError};
use super::{CommandLineProcessor, CommandType};
use crate::utils::{direction_by_abbreviation, is_direction};

#[allow(clippy::enum_variant_names)]
pub enum InterpretationError {
    NoCommand,
    AmbiguousCommand(String),
    UnknownCommand(String),
}

pub fn interpret_command(
    registry: &CommandRegistry,
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
        return Ok(CommandType::Go(""));
    }

    match registry.lookup(command_name) {
        Ok(command_type) => Ok(command_type),
        Err(LookupError::NotFound) => {
            Err(InterpretationError::UnknownCommand(command_name.to_owned()))
        }
        Err(LookupError::NotUnique) => Err(InterpretationError::AmbiguousCommand(
            command_name.to_owned(),
        )),
    }
}
