use std::net::SocketAddr;
use tokio::net::TcpStream;
use warp::ws::WebSocket;

use crate::sessions::{SessionOutput, SessionState};

#[derive(Debug)]
pub struct SessionInputEvent {
    pub session_id: u64,
    pub session_state: SessionState,
    pub source: String,
    pub input: String,
}

#[derive(Debug)]
pub enum SessionEvent {
    IncomingTelnetSession(TcpStream, SocketAddr),
    IncomingWebsocketSession(Box<WebSocket>, SocketAddr),
    SessionOutput(u64, SessionOutput),
    SessionUpdate(u64, SessionState),
}
