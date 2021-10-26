use super::LogMessage;

#[derive(Debug)]
pub struct SessionLogMessage {
    message: String,
}

impl SessionLogMessage {
    pub fn new(source: String, message: String) -> Self {
        Self {
            message: format!("{:16} {}", source, message),
        }
    }
}

impl LogMessage for SessionLogMessage {
    fn get_log(&self) -> &str {
        "sessions"
    }

    fn get_message(&self) -> &str {
        &self.message
    }
}
