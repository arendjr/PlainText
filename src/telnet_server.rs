use futures::StreamExt;
use tokio::net::{TcpListener, TcpStream};
use tokio::sync::mpsc::Sender;

pub async fn serve(port: u16, mut session_tx: Sender<TcpStream>) {
    if let Ok(mut listener) = TcpListener::bind(&format!("0.0.0.0:{}", port)).await {
        tokio::spawn(async move {
            let mut incoming = listener.incoming();
            while let Some(socket_res) = incoming.next().await {
                match socket_res {
                    Ok(socket) => {
                        println!("Accepted connection from {:?}", socket.peer_addr());
                        if let Err(error) = session_tx.send(socket).await {
                            println!("Failed to dispatch session: {:?}", error);
                        }
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
