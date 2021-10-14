use super::{command_line_processor::CommandLineProcessor, command_registry::CommandRegistry};
use crate::actionable_events::ActionDispatcher;

pub struct CommandHelpers<'a> {
    pub action_dispatcher: &'a ActionDispatcher,
    pub admin_command_registry: &'a CommandRegistry,
    pub command_line_processor: &'a mut CommandLineProcessor,
    pub command_registry: &'a CommandRegistry,
}
