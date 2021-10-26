use super::LogMessage;

#[derive(Debug)]
pub struct ErrorLogMessage {
    message: String,
}

impl ErrorLogMessage {
    pub fn new(message: String) -> Self {
        Self { message }
    }
}

impl LogMessage for ErrorLogMessage {
    fn get_log(&self) -> &str {
        "errors"
    }

    fn get_message(&self) -> &str {
        &self.message
    }
}
