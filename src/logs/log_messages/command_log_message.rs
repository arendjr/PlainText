use super::LogMessage;

#[derive(Debug)]
pub struct CommandLogMessage {
    log: String,
    message: String,
}

impl CommandLogMessage {
    pub fn new(player_name: String, message: String) -> Self {
        Self {
            log: format!("commands.{}", player_name),
            message,
        }
    }
}

impl LogMessage for CommandLogMessage {
    fn get_log(&self) -> &str {
        &self.log
    }

    fn get_message(&self) -> &str {
        &self.message
    }
}
