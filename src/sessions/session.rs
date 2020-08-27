use std::io;
use std::io::Write;
use std::net::Shutdown;
use std::net::TcpStream;

use crate::game_object::GameObjectId;

use super::sign_in::SignInState;

const LF: u8 = 10;
const CR: u8 = 13;

const MAX_INPUT_LENGTH: usize = 1024;

#[derive(Clone, Debug)]
pub enum SessionState {
    SessionClosed,
    SigningIn(SignInState),
    SignedIn,
}

pub struct Session {
    pub id: u64,
    player_id: Option<GameObjectId>,
    socket: TcpStream,
    pub state: SessionState,
}

pub enum SessionOutput {
    None,
    Str(&'static str),
    String(String),
    JSON(serde_json::Value),
}

impl Session {
    pub fn close(&mut self) -> io::Result<()> {
        self.socket.shutdown(Shutdown::Both)
    }

    pub fn new(id: u64, socket: TcpStream) -> Self {
        Self {
            id,
            player_id: None,
            socket,
            state: SessionState::SigningIn(SignInState::new()),
        }
    }

    pub fn send(&mut self, data: String) {
        if let Err(error) = self.socket.write(data.as_bytes()) {
            println!("Could not send data over socket: {:?}", error);
        }
    }
}
