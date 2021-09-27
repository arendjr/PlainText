use async_trait::async_trait;
use futures::{stream::SplitSink, FutureExt, SinkExt};
use serde::Serialize;
use std::net::SocketAddr;
use tokio::io::Result;
use warp::ws::{Message, WebSocket};

use super::session::Session;
use super::{SessionOutput, SessionPromptInfo, SessionState, SignInState};

#[derive(Serialize)]
struct Prompt {
    player: SessionPromptInfo,
}

#[derive(Debug)]
pub struct WebSocketSession {
    pub id: u64,
    addr: SocketAddr,
    tx: SplitSink<Box<WebSocket>, Message>,
    state: SessionState,
}

impl WebSocketSession {
    pub fn new(id: u64, tx: SplitSink<Box<WebSocket>, Message>, addr: SocketAddr) -> Self {
        Self {
            id,
            addr,
            tx,
            state: SessionState::SigningIn(Box::new(SignInState::new())),
        }
    }
}

#[async_trait]
impl Session for WebSocketSession {
    async fn close(&mut self) -> Result<()> {
        self.tx.close().map(|_| Ok(())).await
    }

    async fn send(&mut self, output: SessionOutput) {
        match output {
            SessionOutput::Json(data) => {
                send_message(&mut self.tx, Message::text(data.to_string())).await
            }
            SessionOutput::Str(output) => {
                send_message(&mut self.tx, Message::text(output.to_owned())).await
            }
            SessionOutput::String(output) => {
                send_message(&mut self.tx, Message::text(output.to_owned())).await
            }
            SessionOutput::Prompt(info) => {
                let prompt = Prompt { player: info };
                send_message(
                    &mut self.tx,
                    Message::text(serde_json::to_string(&prompt).unwrap()),
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

async fn send_message(tx: &mut SplitSink<Box<WebSocket>, Message>, message: Message) {
    if let Err(error) = tx.send(message).await {
        println!("Could not send data over socket: {:?}", error);
    }
}
