use super::LogMessage;

#[derive(Debug)]
pub struct CommandLogMessage {
    player_name: String,
    message: String,
}

impl CommandLogMessage {
    pub fn new(player_name: String, message: String) -> Self {
        Self {
            player_name,
            message,
        }
    }
}

impl LogMessage for CommandLogMessage {
    fn get_log(&self) -> String {
        format!("commands.{}", self.player_name)
    }

    fn get_message(&self) -> String {
        self.message.clone()
    }
}
