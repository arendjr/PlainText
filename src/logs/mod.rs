use tokio::sync::mpsc::Sender;

mod log_handler;
mod log_messages;
mod log_utils;

pub use log_handler::create_log_handler;
pub use log_messages::LogMessage;
pub use log_utils::*;

pub type LogSender = Sender<Box<dyn LogMessage>>;
