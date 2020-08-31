use futures::{FutureExt, StreamExt};
use std::sync::mpsc::channel;
use std::sync::Arc;
use std::{env, fs, io, thread};
use warp::Filter;

mod character_stats;
mod colors;
mod event_loop;
mod game_object;
mod objects;
mod point3d;
mod session_runner;
mod sessions;
mod telnet_server;
mod text_utils;
mod util;

use event_loop::{InputEvent, SessionEvent, SessionUpdate};
use game_object::{hydrate, GameObject, GameObjectRef, GameObjectType};
use objects::Realm;
use sessions::{process_input, SessionState};

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
        Ok(mut realm) => {
            let (input_tx, input_rx) = channel::<InputEvent>();
            let (session_tx, session_rx) = channel::<SessionEvent>();

            session_runner::create_sessions_handler(input_tx, session_rx);

            let session_tx_clone = session_tx.clone();
            thread::spawn(move || {
                telnet_server::serve(telnet_port, session_tx_clone);
            });
            tokio::spawn(warp::serve(routes).run(([0, 0, 0, 0], websocket_port)));

            println!(
                "Listening for WebSocket connections at port {}.",
                websocket_port
            );

            while let Ok(input_ev) = input_rx.recv() {
                match input_ev.session_state {
                    SessionState::SigningIn(state) => {
                        // println!("state: {:?}, input: {:?}", state, input_ev.input);
                        let (new_state, output) = process_input(&state, &realm, input_ev.input);
                        let session_state = if new_state.is_session_closed() {
                            SessionState::SessionClosed
                        } else if let Some(user_name) = new_state.get_sign_in_user_name() {
                            if let Some(sign_up_data) = new_state.get_sign_up_data() {
                                realm = realm.create_player(sign_up_data);
                                // TODO: logSessionEvent(this._session.source, "Character created for player " + player.name);
                            }

                            if let Some(player) = realm.get_player_by_name(user_name) {
                                let player_id = player.get_id();
                                SessionState::SignedIn(player_id)
                            } else {
                                println!("Could not determine ID for player \"{}\"", user_name);
                                SessionState::SessionClosed
                            }
                        } else {
                            SessionState::SigningIn(new_state)
                        };

                        if let Err(error) =
                            session_tx.send(SessionEvent::SessionUpdate(SessionUpdate {
                                output,
                                session_id: input_ev.session_id,
                                session_state,
                            }))
                        {
                            println!("Could not send output: {:?}", error);
                        }
                    }
                    other_state => panic!("Session state not implemented: {:?}", other_state),
                }
            }
        }
        Err(error) => panic!("Failed to load data from `{}`: {}", data_dir, error),
    }
}

fn load_data(data_dir: &str) -> Result<Realm, io::Error> {
    let content = fs::read_to_string(&format!("{}/realm.000000000", data_dir))?;
    match Realm::hydrate(0, &content).and_then(|object| Ok(object.to_realm())) {
        Ok(Some(mut realm)) => {
            let entries = fs::read_dir(&data_dir)?
                .map(|entry| entry.map(|entry| (entry.file_name(), entry.path())))
                .collect::<Result<Vec<_>, io::Error>>()?;
            for (file_name, path) in entries {
                if let Some(object_ref) = GameObjectRef::from_file_name(&file_name) {
                    let content = fs::read_to_string(&path)?;
                    match hydrate(object_ref, &content) {
                        Ok(object) => realm = realm.set(object_ref, object),
                        Err(message) => println!(
                            "error loading {} {}: {}",
                            object_ref.get_type(),
                            object_ref.get_id(),
                            message
                        ),
                    }
                }
            }

            Ok(inject_players_into_rooms(realm))
        }
        Err(message) => panic!("Failed to load realm: {}", message),
        _ => panic!("Unexpected error loading realm"),
    }
}

// Players get injected on load, so that rooms don't need to be re-persisted every time a character
// moves from one room to another.
fn inject_players_into_rooms(mut realm: Realm) -> Realm {
    for player_id in realm.player_ids() {
        if let Some(room) = realm
            .get_player(player_id)
            .and_then(|player| realm.get_room(player.get_current_room().get_id()))
        {
            let player_ref = GameObjectRef(GameObjectType::Player, player_id);
            realm = realm.set(
                room.get_ref(),
                Arc::new(room.with_characters(vec![player_ref])),
            );
        }
    }
    realm
}
