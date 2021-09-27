use futures::StreamExt;
use std::sync::Arc;
use std::{collections::HashMap, net::SocketAddr};
use tokio::net::TcpStream;
use tokio::sync::mpsc::{Receiver, Sender};
use tokio::sync::Mutex;
use warp::ws::WebSocket;

use crate::logs::{log_session_event, LogMessage};

use super::session::Session;
use super::session_events::{SessionEvent, SessionInputEvent};
use super::telnet_session::TelnetSession;
use super::websocket_session::WebSocketSession;
use super::{SessionReader, SessionState};

type SessionMap = Arc<Mutex<HashMap<u64, Box<dyn Session>>>>;

pub fn create_sessions_handler(
    input_tx: Sender<SessionInputEvent>,
    log_tx: Sender<Box<dyn LogMessage>>,
    mut session_rx: Receiver<SessionEvent>,
) {
    tokio::spawn(async move {
        let session_map: SessionMap = Arc::new(Mutex::new(HashMap::new()));

        let mut next_id = 1;

        while let Some(session_ev) = session_rx.recv().await {
            match session_ev {
                SessionEvent::IncomingTelnetSession(socket, addr) => {
                    let session_id = next_id;
                    next_id += 1;
                    create_telnet_session_handler(
                        session_map.clone(),
                        session_id,
                        socket,
                        addr,
                        input_tx.clone(),
                    )
                }

                SessionEvent::IncomingWebsocketSession(websocket, addr) => {
                    let session_id = next_id;
                    next_id += 1;
                    create_websocket_session_handler(
                        session_map.clone(),
                        session_id,
                        websocket,
                        addr,
                        input_tx.clone(),
                    )
                }

                SessionEvent::SessionOutput(session_id, output) => {
                    if let Some(session) = session_map.lock().await.get_mut(&session_id) {
                        session.send(output).await;
                    }
                }

                SessionEvent::SessionUpdate(session_id, session_state) => {
                    if let Some(session) = session_map.lock().await.get_mut(&session_id) {
                        match session_state {
                            SessionState::SessionClosed(player_id) => {
                                log_session_event(
                                    &log_tx,
                                    session.source(),
                                    "Connection closed".to_owned(),
                                )
                                .await;
                                session.set_state(SessionState::SessionClosed(player_id));
                                if let Err(error) = session.close().await {
                                    println!("Could not close session: {:?}", error);
                                }
                            }
                            other_state => session.set_state(other_state),
                        }
                    }
                }
            }
        }
    });
}

fn create_telnet_session_handler(
    session_map: SessionMap,
    session_id: u64,
    socket: TcpStream,
    addr: SocketAddr,
    input_tx: Sender<SessionInputEvent>,
) {
    tokio::spawn(async move {
        let (reader, writer) = socket.into_split();
        let mut reader = SessionReader::new(reader);

        let session = TelnetSession::new(session_id, writer, addr);
        send_event(
            &input_tx,
            session_id,
            session.state().clone(),
            session.source(),
            String::new(),
        )
        .await;
        session_map
            .lock()
            .await
            .insert(session_id, Box::new(session));

        while let Some(line) = reader.read_line().await {
            if let Some(session) = session_map.lock().await.get_mut(&session_id) {
                send_event(
                    &input_tx,
                    session_id,
                    session.state().clone(),
                    session.source(),
                    line,
                )
                .await;
            }
        }

        close_session(session_map, input_tx, session_id).await;
    });
}

fn create_websocket_session_handler(
    session_map: SessionMap,
    session_id: u64,
    websocket: Box<WebSocket>,
    addr: SocketAddr,
    input_tx: Sender<SessionInputEvent>,
) {
    tokio::spawn(async move {
        let (tx, mut rx) = websocket.split();

        let session = WebSocketSession::new(session_id, tx, addr);
        send_event(
            &input_tx,
            session_id,
            session.state().clone(),
            session.source(),
            String::new(),
        )
        .await;
        session_map
            .lock()
            .await
            .insert(session_id, Box::new(session));

        while let Some(input_res) = rx.next().await {
            match input_res {
                Ok(input_message) => {
                    if let Ok(input) = input_message.to_str() {
                        if let Some(session) = session_map.lock().await.get_mut(&session_id) {
                            send_event(
                                &input_tx,
                                session_id,
                                session.state().clone(),
                                session.source(),
                                input.to_owned(),
                            )
                            .await;
                        }
                    }
                }
                Err(error) => println!("Error receiving WebSocket message: {:?}", error),
            }
        }

        close_session(session_map, input_tx, session_id).await;
    });
}

async fn close_session(
    session_map: SessionMap,
    input_tx: Sender<SessionInputEvent>,
    session_id: u64,
) {
    let mut session_map = session_map.lock().await;
    if let Some(session) = session_map.get_mut(&session_id) {
        if let Err(error) = session.close().await {
            println!("Error closing socket: {:?}", error);
        }

        let player_id = match session.state() {
            SessionState::SigningIn(_) => None,
            SessionState::SignedIn(player_id) => Some(*player_id),
            SessionState::SessionClosed(player_id) => *player_id,
        };
        send_event(
            &input_tx,
            session_id,
            SessionState::SessionClosed(player_id),
            session.source(),
            String::new(),
        )
        .await;

        session_map.remove(&session_id);
    }
}

async fn send_event(
    input_tx: &Sender<SessionInputEvent>,
    session_id: u64,
    session_state: SessionState,
    source: String,
    input: String,
) {
    if let Err(error) = input_tx
        .send(SessionInputEvent {
            session_id,
            session_state,
            source,
            input,
        })
        .await
    {
        println!("Could not send event: {:?}", error);
    }
}
