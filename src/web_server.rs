use futures::{FutureExt, StreamExt};
use std::fs;
use warp::{http::Response, Filter};

pub fn serve(realm_name: String, port: u16) {
    let index_html = fs::read_to_string("web/index.html").unwrap();

    let ws_upgrade = warp::ws().map(|ws: warp::ws::Ws| {
        ws.on_upgrade(|websocket| {
            // Just echo all messages back...
            let (tx, rx) = websocket.split();
            rx.forward(tx).map(|result| {
                if let Err(e) = result {
                    eprintln!("websocket error: {:?}", e);
                }
            })
        })
    });
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
