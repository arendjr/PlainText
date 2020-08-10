use futures::StreamExt;
use std::sync::Mutex;
use tokio::net::TcpListener;

use crate::game_object::GameObjectMapReader;
use crate::sessions::{Session, SessionManager};
use crate::transaction_writer::TransactionWriter;

pub async fn serve(
    port: u16,
    game_object_reader: GameObjectMapReader,
    session_manager: Mutex<SessionManager>,
    transaction_writer: TransactionWriter,
) {
    if let Ok(mut listener) = TcpListener::bind(&format!("0.0.0.0:{}", port)).await {
        tokio::spawn(async move {
            let mut incoming = listener.incoming();
            while let Some(socket_res) = incoming.next().await {
                match socket_res {
                    Ok(socket) => {
                        println!("Accepted connection from {:?}", socket.peer_addr());
                        match session_manager.try_lock() {
                            Ok(mut session_manager) => {
                                let session_id = session_manager.get_next_id();
                                let session = Session::new(
                                    session_id,
                                    socket,
                                    game_object_reader.clone(),
                                    transaction_writer.clone(),
                                );
                                session_manager.register_session(session);
                            }
                            Err(error) => {
                                panic!("Could not acquire session manager: {:?}", error);
                            }
                        };
                    }
                    Err(error) => {
                        println!("Telnet connection error: {:?}", error);
                    }
                }
            }
        });

        println!("Listening for telnet connections on port {}.", port);
    } else {
        panic!("Could not bind to port {}", port);
    }
}
