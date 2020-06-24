use futures::{FutureExt, StreamExt};
use std::sync::Arc;
use std::{env, fs, io};
use warp::Filter;

mod character_stats;
mod game_object;
mod objects;
mod point3d;
mod util;

use game_object::{hydrate, GameObject, GameObjectRef, GameObjectType};

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
        Ok((game_object_reader, mut game_object_writer)) => {
            println!("Serving at port {}", websocket_port);

            warp::serve(routes)
                .run(([0, 0, 0, 0], websocket_port))
                .await;
        }
        Err(error) => panic!("Failed to load data from `{}`: {}", data_dir, error),
    }
}

fn load_data(
    data_dir: &str,
) -> Result<
    (
        evmap::ReadHandle<GameObjectRef, Arc<dyn GameObject>>,
        evmap::WriteHandle<GameObjectRef, Arc<dyn GameObject>>,
    ),
    io::Error,
> {
    let (game_object_reader, mut game_object_writer) = game_object::new_map();

    let mut player_refs = vec![];

    let entries = fs::read_dir(&data_dir)?
        .map(|entry| entry.map(|entry| (entry.file_name(), entry.path())))
        .collect::<Result<Vec<_>, io::Error>>()?;
    for (file_name, path) in entries {
        if let Some(object_ref) = GameObjectRef::from_file_name(&file_name) {
            let content = fs::read_to_string(&path)?;
            match hydrate(object_ref, &content) {
                Ok(object) => {
                    game_object_writer.insert(object_ref, object);

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

    game_object_writer.refresh();

    for player_ref in player_refs {
        if let Some(player_object) = game_object_reader.get_one(&player_ref) {
            if let Some(room_object) = player_object
                .to_player()
                .map(|player| player.get_current_room())
                .and_then(|room_ref| game_object_reader.get_one(&room_ref))
            {
                if let Some(room) = room_object.to_room() {
                    game_object_writer.update(
                        room_object.get_ref(),
                        Arc::new(room.with_characters(vec![player_ref])),
                    );
                    game_object_writer.refresh();
                }
            }
        }
    }

    game_object_writer.refresh();

    Ok((game_object_reader, game_object_writer))
}
