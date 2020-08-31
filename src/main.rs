use futures::{FutureExt, StreamExt};
use std::sync::mpsc::{channel, Sender};
use std::sync::Arc;
use std::{env, fs, io, thread};
use warp::Filter;

mod character_stats;
mod colors;
mod event_loop;
mod game_object;
mod logs;
mod objects;
mod point3d;
mod sessions;
mod telnet_server;
mod text_utils;
mod util;

use event_loop::{InputEvent, SessionEvent};
use game_object::{hydrate, GameObject, GameObjectRef, GameObjectType};
use logs::{log_command, log_session_event, LogMessage};
use objects::Realm;
use sessions::{process_input, SessionState};

#[tokio::main]
async fn main() {
    let data_dir = env::var("PT_DATA_DIR").unwrap_or("data/".to_owned());
    let log_dir = env::var("PT_LOG_DIR").unwrap_or("logs/".to_owned());

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
            let (log_tx, log_rx) = channel::<Box<dyn LogMessage>>();

            logs::create_log_thread(log_dir, log_rx);
            sessions::create_sessions_thread(input_tx, log_tx.clone(), session_rx);

            let session_tx_clone = session_tx.clone();
            thread::spawn(move || {
                telnet_server::serve(telnet_port, session_tx_clone);
            });
            tokio::spawn(warp::serve(routes).run(([0, 0, 0, 0], websocket_port)));

            println!(
                "Listening for WebSocket connections at port {}.",
                websocket_port
            );

            while let Ok(InputEvent {
                input,
                session_id,
                session_state,
                source,
            }) = input_rx.recv()
            {
                match session_state {
                    SessionState::SigningIn(state) => {
                        // println!("state: {:?}, input: {:?}", state, input_ev.input);
                        let (new_state, output) =
                            process_input(&state, &realm, &log_tx, &source, input);
                        send_session_event(
                            &session_tx,
                            SessionEvent::SessionOutput(session_id, output),
                        );

                        let session_state = if new_state.is_session_closed() {
                            SessionState::SessionClosed(None)
                        } else if let Some(user_name) = new_state.get_sign_in_user_name() {
                            if let Some(sign_up_data) = new_state.get_sign_up_data() {
                                realm = realm.create_player(sign_up_data);
                                log_session_event(
                                    &log_tx,
                                    source.clone(),
                                    format!("Character created for player \"{}\"", user_name),
                                );
                            }

                            if let Some(mut player) = realm.get_player_by_name(user_name) {
                                if let Some(existing_session_id) = player.get_session_id() {
                                    send_session_event(
                                        &session_tx,
                                        SessionEvent::SessionUpdate(
                                            existing_session_id,
                                            SessionState::SessionClosed(Some(player.get_id())),
                                        ),
                                    );
                                }

                                let player_id = player.get_id();
                                player.set_session_id(Some(session_id));
                                realm = realm.set(player.get_ref(), Arc::new(player));

                                log_command(
                                    &log_tx,
                                    user_name.to_owned(),
                                    "(signed in)".to_owned(),
                                );

                                SessionState::SignedIn(player_id)
                            } else {
                                log_session_event(
                                    &log_tx,
                                    source,
                                    format!("Could not determine ID for player \"{}\"", user_name),
                                );
                                SessionState::SessionClosed(None)
                            }
                        } else {
                            SessionState::SigningIn(new_state)
                        };

                        send_session_event(
                            &session_tx,
                            SessionEvent::SessionUpdate(session_id, session_state),
                        );
                    }

                    SessionState::SignedIn(player_id) => {
                        if let Some(player) = realm.get_player(player_id) {
                            log_command(&log_tx, player.get_name().to_owned(), input.clone());
                        } else {
                            log_session_event(
                                &log_tx,
                                source,
                                format!("Player {} deleted", player_id),
                            );
                            send_session_event(
                                &session_tx,
                                SessionEvent::SessionUpdate(
                                    session_id,
                                    SessionState::SessionClosed(Some(player_id)),
                                ),
                            );
                        }
                    }

                    SessionState::SessionClosed(player_id) => {
                        if let Some(player_id) = player_id {
                            if let Some(mut player) = realm.get_player(player_id) {
                                let player_name = player.get_name().to_owned();
                                player.set_session_id(None);
                                realm = realm.set(player.get_ref(), Arc::new(player));

                                log_command(&log_tx, player_name, "(session closed)".to_owned());
                            }
                        }
                    }
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

fn send_session_event(session_tx: &Sender<SessionEvent>, event: SessionEvent) {
    if let Err(error) = session_tx.send(event) {
        println!("Could not send session event: {:?}", error);
    }
}
