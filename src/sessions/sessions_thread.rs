use std::collections::HashMap;
use std::io::ErrorKind;
use std::net::TcpStream;
use std::sync::mpsc::{Receiver, Sender};
use std::sync::{Arc, Mutex};
use std::thread;

use crate::logs::{log_session_event, LogMessage};

use super::session::Session;
use super::session_events::{SessionEvent, SessionInputEvent};
use super::telnet_session::TelnetSession;
use super::{SessionReader, SessionState};

type SessionMap = Arc<Mutex<HashMap<u64, Box<dyn Session>>>>;

pub fn create_sessions_thread(
    input_tx: Sender<SessionInputEvent>,
    log_tx: Sender<Box<dyn LogMessage>>,
    session_rx: Receiver<SessionEvent>,
) {
    thread::spawn(move || {
        let session_map: SessionMap = Arc::new(Mutex::new(HashMap::new()));

        let mut next_id = 1;

        while let Ok(session_ev) = session_rx.recv() {
            match session_ev {
                SessionEvent::IncomingSession(socket) => {
                    let session_id = next_id;
                    next_id += 1;
                    create_session_thread(session_map.clone(), session_id, socket, input_tx.clone())
                }

                SessionEvent::SessionOutput(session_id, output) => {
                    if let Some(session) = session_map.lock().unwrap().get_mut(&session_id) {
                        session.send(output);
                    }
                }

                SessionEvent::SessionUpdate(session_id, session_state) => {
                    if let Some(session) = session_map.lock().unwrap().get_mut(&session_id) {
                        match session_state {
                            SessionState::SessionClosed(player_id) => {
                                log_session_event(
                                    &log_tx,
                                    session.source(),
                                    "Connection closed".to_owned(),
                                );
                                session.set_state(SessionState::SessionClosed(player_id));
                                if let Err(error) = session.close() {
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

fn create_session_thread(
    session_map: SessionMap,
    session_id: u64,
    socket: TcpStream,
    input_tx: Sender<SessionInputEvent>,
) {
    thread::spawn(move || {
        let mut reader = SessionReader::new(socket.try_clone().unwrap());

        let session = TelnetSession::new(session_id, socket);
        send_event(
            &input_tx,
            session_id,
            session.state().clone(),
            session.source(),
            "".to_owned(),
        );
        session_map
            .lock()
            .unwrap()
            .insert(session_id, Box::new(session));

        while let Some(line) = reader.read_line() {
            if let Some(session) = session_map.lock().unwrap().get_mut(&session_id) {
                send_event(
                    &input_tx,
                    session_id,
                    session.state().clone(),
                    session.source(),
                    line,
                );
            }
        }

        if let Err(error) = reader.close() {
            if error.kind() != ErrorKind::NotConnected {
                println!("Error closing socket: {:?}", error);
            }
        }

        if let Ok(mut session_map) = session_map.lock() {
            if let Some(session) = session_map.get(&session_id) {
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
                    "".to_owned(),
                );

                session_map.remove(&session_id);
            }
        }
    });
}

fn send_event(
    input_tx: &Sender<SessionInputEvent>,
    session_id: u64,
    session_state: SessionState,
    source: String,
    input: String,
) {
    if let Err(error) = input_tx.send(SessionInputEvent {
        session_id,
        session_state,
        source,
        input,
    }) {
        println!("Could not send event: {:?}", error);
    }
}
