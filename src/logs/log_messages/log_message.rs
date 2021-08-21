use core::fmt::Debug;

pub trait LogMessage: Debug + Send + Sync {
    /**
     * The log to log the message to.
     */
    fn get_log(&self) -> String;

    /**
     * The message to log.
     */
    fn get_message(&self) -> String;
}
