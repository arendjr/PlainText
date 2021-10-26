use core::fmt::Debug;

pub trait LogMessage: Debug + Send + Sync {
    /**
     * The log to log the message to.
     */
    fn get_log(&self) -> &str;

    /**
     * The message to log.
     */
    fn get_message(&self) -> &str;
}
