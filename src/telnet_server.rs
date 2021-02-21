use tokio::net::TcpListener;
use tokio::sync::mpsc::Sender;

use crate::sessions::SessionEvent;

pub fn serve(port: u16, session_tx: Sender<SessionEvent>) {
    tokio::spawn(async move {
        let listener = TcpListener::bind(&format!("0.0.0.0:{}", port))
            .await
            .unwrap_or_else(|error| panic!("Could not bind to port {}: {:?}", port, error));

        println!("Listening for telnet connections on port {}.", port);

        while let Ok((stream, addr)) = listener.accept().await {
            if let Err(error) = session_tx
                .send(SessionEvent::IncomingTelnetSession(stream, addr))
                .await
            {
                println!("Failed to dispatch session: {:?}", error);
            }
        }
    });
}
