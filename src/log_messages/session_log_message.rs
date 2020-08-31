use super::LogMessage;

pub struct SessionLogMessage {
    source: String,
    message: String,
}

impl SessionLogMessage {
    pub fn new(source: String, message: String) -> Self {
        Self { source, message }
    }
}

impl LogMessage for SessionLogMessage {
    fn get_log(&self) -> String {
        "sessions".to_owned()
    }

    fn get_message(&self) -> String {
        format!("{:16} {}", self.source, self.message)
    }
}
