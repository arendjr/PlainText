use crate::trigger_registry::TriggerRegistry;

use super::{command_line_processor::CommandLineProcessor, command_registry::CommandRegistry};

pub struct CommandHelpers<'a> {
    pub admin_command_registry: &'a CommandRegistry,
    pub command_line_processor: &'a mut CommandLineProcessor,
    pub command_registry: &'a CommandRegistry,
    pub trigger_registry: &'a TriggerRegistry,
}
