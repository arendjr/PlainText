use std::net::TcpStream;

use crate::sessions::{SessionOutput, SessionState};

pub struct InputEvent {
    pub session_id: u64,
    pub session_state: SessionState,
    pub input: String,
}

pub enum SessionEvent {
    IncomingSession(TcpStream),
    SessionUpdate(SessionUpdate),
}

pub struct SessionUpdate {
    pub output: SessionOutput,
    pub session_id: u64,
    pub session_state: SessionState,
}
