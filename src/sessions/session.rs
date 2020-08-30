use std::io;
use std::io::Write;
use std::net::Shutdown;
use std::net::TcpStream;

use crate::game_object::GameObjectId;

use super::session_output::SessionOutput;
use super::sign_in::SignInState;

const LF: u8 = 10;
const CR: u8 = 13;

const MAX_INPUT_LENGTH: usize = 1024;

#[derive(Clone, Debug)]
pub enum SessionState {
    SessionClosed,
    SigningIn(SignInState),
    SignedIn(GameObjectId),
}

pub struct Session {
    pub id: u64,
    socket: TcpStream,
    pub state: SessionState,
}

impl Session {
    pub fn close(&mut self) -> io::Result<()> {
        self.socket.shutdown(Shutdown::Both)
    }

    pub fn new(id: u64, socket: TcpStream) -> Self {
        Self {
            id,
            socket,
            state: SessionState::SigningIn(SignInState::new()),
        }
    }

    pub fn send(&mut self, output: SessionOutput) {
        match output {
            SessionOutput::JSON(_) => {}
            SessionOutput::Str(output) => send_data(&mut self.socket, output.as_bytes()),
            SessionOutput::String(output) => send_data(&mut self.socket, output.as_bytes()),
            SessionOutput::Aggregate(outputs) => {
                for output in outputs {
                    self.send(output)
                }
            }
            SessionOutput::None => {}
        }
    }
}

fn send_data(socket: &mut TcpStream, data: &[u8]) {
    if let Err(error) = socket.write(data) {
        println!("Could not send data over socket: {:?}", error);
    }
}
