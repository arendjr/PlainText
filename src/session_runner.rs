use std::collections::HashMap;
use std::sync::atomic::{AtomicU64, Ordering};
use tokio::net::TcpStream;
use tokio::sync::mpsc::{channel, Sender};

use crate::sessions::Session;

pub fn process_sessions() -> Sender<TcpStream> {
    let (socket_tx, mut socket_rx) = channel::<TcpStream>(100);

    let mut session_map: HashMap<u64, Session> = HashMap::new();

    tokio::spawn(async move {
        let next_id = AtomicU64::new(1);

        while let Some(socket) = socket_rx.recv().await {
            let session_id = next_id.fetch_add(1, Ordering::Relaxed);
            let mut session = Session::new(session_id, socket);

            tokio::spawn(async move {
                let output = process_session(&session, "".to_owned());
                session.send(output).await;

                while let Some(line) = session.read_line().await {
                    let output = process_session(&session, line);
                    session.send(output).await;
                }

                if let Err(error) = session.close().await {
                    println!("Error closing socket: {:?}", error);
                }
            });
        }
    });

    socket_tx
}

fn process_session(session: &Session, input: String) -> String {
    format!("Hello {}: {}\n", session.id, input)
}
