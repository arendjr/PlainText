use async_trait::async_trait;
use std::net::SocketAddr;
use tokio::io::{AsyncWriteExt, Result};
use tokio::net::tcp::OwnedWriteHalf;

use super::session::Session;
use super::{SessionOutput, SessionState, SignInState};

#[derive(Debug)]
pub struct TelnetSession {
    pub id: u64,
    addr: SocketAddr,
    writer: OwnedWriteHalf,
    state: SessionState,
}

impl TelnetSession {
    pub fn new(id: u64, writer: OwnedWriteHalf, addr: SocketAddr) -> Self {
        Self {
            id,
            addr,
            writer,
            state: SessionState::SigningIn(SignInState::new()),
        }
    }
}

#[async_trait]
impl Session for TelnetSession {
    async fn close(&mut self) -> Result<()> {
        self.writer.shutdown().await
    }

    async fn send(&mut self, output: SessionOutput) {
        match output {
            SessionOutput::Json(_) => {}
            SessionOutput::Str(output) => send_data(&mut self.writer, output.as_bytes()).await,
            SessionOutput::String(output) => send_data(&mut self.writer, output.as_bytes()).await,
            SessionOutput::Prompt(info) => {
                send_data(
                    &mut self.writer,
                    format!("({}H {}M) ", info.hp, info.mp).as_bytes(),
                )
                .await
            }
            SessionOutput::Aggregate(outputs) => {
                for output in outputs {
                    self.send(output).await
                }
            }
            SessionOutput::None => {}
        }
    }

    fn set_state(&mut self, state: SessionState) {
        self.state = state;
    }

    fn source(&self) -> String {
        format!("{}", self.addr.ip())
    }

    fn state(&self) -> &SessionState {
        &self.state
    }
}

async fn send_data(writer: &mut OwnedWriteHalf, data: &[u8]) {
    if let Err(error) = writer.write(data).await {
        println!("Could not send data over socket: {:?}", error);
    }
}
