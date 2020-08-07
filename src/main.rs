use futures::{FutureExt, StreamExt};
use std::sync::{Arc, Mutex};
use std::{env, fs, io};
use warp::Filter;

mod character_stats;
mod game_object;
mod objects;
mod point3d;
mod sessions;
mod telnet_server;
mod transaction_writer;
mod util;

use game_object::{hydrate, GameObject, GameObjectMap, GameObjectRef, GameObjectType};
use sessions::SessionManager;
use transaction_writer::TransactionWriter;

#[tokio::main]
async fn main() {
    let data_dir = env::var("PT_DATA_DIR").unwrap_or("data/".to_owned());

    let telnet_port = env::var("PT_TELNET_PORT")
        .unwrap_or("".to_string())
        .parse()
        .unwrap_or(4801);
    let websocket_port = env::var("PT_WEBSOCKET_PORT")
        .unwrap_or("".to_string())
        .parse()
        .unwrap_or(4802);
    let http_port = env::var("PT_HTTP_PORT")
        .unwrap_or("".to_string())
        .parse()
        .unwrap_or(8080);

    let routes = warp::any().and(warp::ws()).map(|ws: warp::ws::Ws| {
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

    match load_data(&data_dir) {
        Ok((game_object_reader, game_object_writer)) => {
            let session_manager = Mutex::new(SessionManager::new());
            let transaction_writer =
                TransactionWriter::new(game_object_reader.clone(), game_object_writer);

            let mut handles = vec![];
            handles.push(tokio::spawn(telnet_server::serve(
                telnet_port,
                game_object_reader,
                session_manager,
                transaction_writer,
            )));

            handles.push(tokio::spawn(
                warp::serve(routes).run(([0, 0, 0, 0], websocket_port)),
            ));
            println!(
                "Listening for WebSocket connections at port {}.",
                websocket_port
            );

            futures::future::join_all(handles).await;
        }
        Err(error) => panic!("Failed to load data from `{}`: {}", data_dir, error),
    }
}

fn load_data(data_dir: &str) -> Result<GameObjectMap, io::Error> {
    let (object_map_reader, mut object_map_writer) = game_object::new_map();

    let mut player_refs = vec![];

    let entries = fs::read_dir(&data_dir)?
        .map(|entry| entry.map(|entry| (entry.file_name(), entry.path())))
        .collect::<Result<Vec<_>, io::Error>>()?;
    for (file_name, path) in entries {
        if let Some(object_ref) = GameObjectRef::from_file_name(&file_name) {
            let content = fs::read_to_string(&path)?;
            match hydrate(object_ref, &content) {
                Ok(object) => {
                    object_map_writer.set(object_ref, object);

                    if object_ref.get_type() == GameObjectType::Player {
                        player_refs.push(object_ref);
                    }
                }
                Err(message) => println!(
                    "error loading {} {}: {}",
                    object_ref.get_type(),
                    object_ref.get_id(),
                    message
                ),
            }
        }
    }

    object_map_writer.refresh();

    for player_ref in player_refs {
        if let Some(room) = object_map_reader
            .get_player(player_ref.get_id())
            .and_then(|player| object_map_reader.get_room(player.get_current_room().get_id()))
        {
            object_map_writer.set(
                room.get_ref(),
                Arc::new(room.with_characters(vec![player_ref])),
            );
            object_map_writer.refresh();
        }
    }

    object_map_writer.refresh();

    Ok((object_map_reader, object_map_writer))
}
