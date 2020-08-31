use std::net::TcpStream;

use crate::sessions::{SessionOutput, SessionState};

pub struct InputEvent {
    pub session_id: u64,
    pub session_state: SessionState,
    pub source: String,
    pub input: String,
}

pub enum SessionEvent {
    IncomingSession(TcpStream),
    SessionOutput(u64, SessionOutput),
    SessionUpdate(u64, SessionState),
}
