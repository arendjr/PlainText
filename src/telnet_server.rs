use std::net::TcpListener;
use std::sync::mpsc::Sender;
use std::thread;

use crate::sessions::SessionEvent;

pub fn serve(port: u16, session_tx: Sender<SessionEvent>) {
    if let Ok(listener) = TcpListener::bind(&format!("0.0.0.0:{}", port)) {
        thread::spawn(move || {
            let mut incoming = listener.incoming();
            while let Some(socket_res) = incoming.next() {
                match socket_res {
                    Ok(socket) => {
                        if let Err(error) = session_tx.send(SessionEvent::IncomingSession(socket)) {
                            println!("Failed to dispatch session: {:?}", error);
                        }
                    }
                    Err(error) => println!("Telnet connection error: {:?}", error),
                }
            }
        });

        println!("Listening for telnet connections on port {}.", port);
    } else {
        panic!("Could not bind to port {}", port);
    }
}
