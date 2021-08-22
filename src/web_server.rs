use std::convert::Infallible;
use std::fs;
use std::net::{IpAddr, Ipv4Addr, SocketAddr};
use tokio::sync::mpsc::Sender;
use warp::{http::Response, ws::WebSocket, Filter};

use crate::sessions::SessionEvent;

pub fn serve(realm_name: String, port: u16, session_tx: Sender<SessionEvent>) {
    let index_html = fs::read_to_string("web/index.html").unwrap();

    let ws_upgrade = warp::ws()
        .and(warp::addr::remote())
        .and(with_session_tx(session_tx))
        .map(
            |ws: warp::ws::Ws, maybe_addr: Option<SocketAddr>, session_tx: Sender<SessionEvent>| {
                ws.on_upgrade(move |websocket| {
                    let addr = maybe_addr.unwrap_or_else(|| {
                        SocketAddr::new(IpAddr::V4(Ipv4Addr::new(0, 0, 0, 0)), 0)
                    });
                    send_incoming_session(session_tx, websocket, addr)
                })
            },
        );
    let index = warp::get().and(warp::path::end()).map(move || {
        Response::builder()
            .header("Content-type", "text/html")
            .body(index_html.replace("{{title}}", &realm_name))
    });
    let statics = warp::get().and(warp::fs::dir("./web/"));
    let routes = warp::any().and(ws_upgrade.or(index).or(statics));
    tokio::spawn(warp::serve(routes).run(([0, 0, 0, 0], port)));

    println!("Listening for HTTP connections at port {}.", port);
}

async fn send_incoming_session(
    session_tx: Sender<SessionEvent>,
    websocket: WebSocket,
    addr: SocketAddr,
) {
    if let Err(error) = session_tx
        .send(SessionEvent::IncomingWebsocketSession(websocket, addr))
        .await
    {
        println!("Failed to dispatch session: {:?}", error);
    }
}

fn with_session_tx(
    session_tx: Sender<SessionEvent>,
) -> impl Filter<Extract = (Sender<SessionEvent>,), Error = Infallible> + Clone {
    warp::any().map(move || session_tx.clone())
}
