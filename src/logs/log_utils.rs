use super::log_messages::{CommandLogMessage, SessionLogMessage};
use super::LogSender;

pub async fn log_command(log_tx: &LogSender, player_name: String, message: String) {
    if let Err(error) = log_tx
        .send(Box::new(CommandLogMessage::new(player_name, message)))
        .await
    {
        println!("Could not send log message: {:?}", error);
    }
}

pub async fn log_session_event(log_tx: &LogSender, source: String, message: String) {
    if let Err(error) = log_tx
        .send(Box::new(SessionLogMessage::new(source, message)))
        .await
    {
        println!("Could not send log message: {:?}", error);
    }
}
