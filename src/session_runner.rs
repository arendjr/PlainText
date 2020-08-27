use std::collections::HashMap;
use std::net::TcpStream;
use std::sync::mpsc::{Receiver, Sender};
use std::sync::{Arc, Mutex};
use std::thread;

use crate::event_loop::{InputEvent, SessionEvent};
use crate::sessions::{Session, SessionOutput, SessionReader, SessionState};

type SessionMap = Arc<Mutex<HashMap<u64, Session>>>;

pub fn create_sessions_handler(input_tx: Sender<InputEvent>, session_rx: Receiver<SessionEvent>) {
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
                SessionEvent::SessionUpdate(update) => {
                    if let Some(mut session) =
                        session_map.lock().unwrap().get_mut(&update.session_id)
                    {
                        for output in update.output {
                            match output {
                                SessionOutput::JSON(_) => {}
                                SessionOutput::Str(output) => session.send(output.to_owned()),
                                SessionOutput::String(output) => session.send(output),
                                SessionOutput::None => {}
                            }
                        }

                        session.state = update.session_state;
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
    input_tx: Sender<InputEvent>,
) {
    thread::spawn(move || {
        let mut reader = SessionReader::new(socket.try_clone().unwrap());

        let session = Session::new(session_id, socket);
        send_event(&input_tx, session_id, session.state.clone(), "".to_owned());
        session_map.lock().unwrap().insert(session_id, session);

        while let Some(line) = reader.read_line() {
            if let Some(session) = session_map.lock().unwrap().get_mut(&session_id) {
                send_event(&input_tx, session_id, session.state.clone(), line);
            }
        }

        if let Err(error) = reader.close() {
            println!("Error closing socket: {:?}", error);
        } else {
            println!("Session closed");
        }
    });
}

fn send_event(
    input_tx: &Sender<InputEvent>,
    session_id: u64,
    session_state: SessionState,
    input: String,
) {
    if let Err(error) = input_tx.send(InputEvent {
        session_id,
        session_state,
        input,
    }) {
        println!("Could not send event: {:?}", error);
    }
}
