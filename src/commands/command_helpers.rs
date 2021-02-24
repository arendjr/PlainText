use crate::trigger_registry::TriggerRegistry;

use super::command_line_processor::CommandLineProcessor;

pub struct CommandHelpers<'a> {
    pub command_line_processor: &'a mut CommandLineProcessor,
    pub trigger_registry: &'a TriggerRegistry,
}
