use std::io;
use std::io::Write;
use std::net::Shutdown;
use std::net::TcpStream;

use super::session::Session;
use super::{SessionOutput, SessionState, SignInState};

pub struct TelnetSession {
    pub id: u64,
    pub socket: TcpStream,
    pub state: SessionState,
}

impl TelnetSession {
    pub fn new(id: u64, socket: TcpStream) -> Self {
        Self {
            id,
            socket,
            state: SessionState::SigningIn(SignInState::new()),
        }
    }
}

impl Session for TelnetSession {
    fn close(&mut self) -> io::Result<()> {
        self.socket.shutdown(Shutdown::Both)
    }

    fn send(&mut self, output: SessionOutput) {
        match output {
            SessionOutput::JSON(_) => {}
            SessionOutput::Str(output) => send_data(&mut self.socket, output.as_bytes()),
            SessionOutput::String(output) => send_data(&mut self.socket, output.as_bytes()),
            SessionOutput::Prompt(info) => send_data(
                &mut self.socket,
                format!("({}H {}M) ", info.hp, info.mp).as_bytes(),
            ),
            SessionOutput::Aggregate(outputs) => {
                for output in outputs {
                    self.send(output)
                }
            }
            SessionOutput::None => {}
        }
    }

    fn set_state(&mut self, state: SessionState) {
        self.state = state;
    }

    fn source(&self) -> String {
        match self.socket.peer_addr() {
            Ok(addr) => format!("{}", addr.ip()),
            Err(_) => "(unknown source)".to_owned(),
        }
    }

    fn state(&self) -> &SessionState {
        &self.state
    }
}

fn send_data(socket: &mut TcpStream, data: &[u8]) {
    if let Err(error) = socket.write(data) {
        println!("Could not send data over socket: {:?}", error);
    }
}
