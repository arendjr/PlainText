use futures::{FutureExt, StreamExt};
use std::sync::mpsc::{channel, Sender};
use std::{env, fs, io, thread};
use warp::Filter;

#[macro_use]
mod serializable_flags;

macro_rules! unwrap_or_continue {
    ($expr:expr) => {
        match $expr {
            Some(value) => value,
            None => continue,
        }
    };
}

macro_rules! unwrap_or_return {
    ($expr:expr, $ret:expr) => {
        match $expr {
            Some(value) => value,
            None => return $ret,
        }
    };
}

mod actions;
mod character_stats;
mod colors;
mod command_interpreter;
mod commands;
mod direction_utils;
mod events;
mod game_object;
mod logs;
mod objects;
mod persistence_thread;
mod player_output;
mod point3d;
mod sessions;
mod telnet_server;
mod text_utils;
mod vector3d;
mod vector_utils;
mod visual_utils;

use actions::{enter, look};
use command_interpreter::{interpret_command, InterpretationError};
use commands::execute_command;
use game_object::{hydrate, Character, GameObject, GameObjectId, GameObjectRef};
use logs::{log_command, log_session_event, LogMessage, LogSender};
use objects::Realm;
use persistence_thread::PersistenceRequest;
use player_output::PlayerOutput;
use sessions::{
    process_input, SessionEvent, SessionInputEvent, SessionOutput, SessionPromptInfo, SessionState,
    SignInState,
};

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
    /* TODO: let http_port = env::var("PT_HTTP_PORT")
    .unwrap_or("".to_string())
    .parse()
    .unwrap_or(8080);*/

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

    let mut realm = match load_data(&data_dir) {
        Ok(realm) => realm,
        Err(error) => panic!("Failed to load data from \"{}\": {}", data_dir, error),
    };

    let (input_tx, input_rx) = channel::<SessionInputEvent>();
    let (persist_tx, persist_rx) = channel::<PersistenceRequest>();
    let (session_tx, session_rx) = channel::<SessionEvent>();
    let (log_tx, log_rx) = channel::<Box<dyn LogMessage>>();

    logs::create_log_thread(log_dir, log_rx);
    persistence_thread::create_persistence_thread(data_dir, persist_rx);
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

    while let Ok(SessionInputEvent {
        input,
        session_id,
        session_state,
        source,
    }) = input_rx.recv()
    {
        match session_state {
            SessionState::SigningIn(state) => {
                let input_ev = (input, session_id, source, state);
                realm = process_signing_in_input(realm, &session_tx, &log_tx, input_ev);
            }
            SessionState::SignedIn(player_id) => {
                let input_ev = (input, session_id, source, player_id);
                realm = process_signed_in_input(realm, &session_tx, &log_tx, input_ev);
            }
            SessionState::SessionClosed(player_id) => {
                realm = process_session_closed(realm, &session_tx, &log_tx, player_id);
            }
        }

        let persistence_requests = realm.take_persistence_requests();
        for request in persistence_requests {
            if let Err(error) = persist_tx.send(request) {
                panic!("Couldn't send persistence request: {:?}", error);
            }
        }
    }
}

fn load_data(data_dir: &str) -> Result<Realm, io::Error> {
    let content = fs::read_to_string(&format!("{}/realm.000000000", data_dir))?;
    let mut realm = Realm::hydrate(0, &content)
        .unwrap_or_else(|message| panic!("Failed to load realm: {}", message))
        .as_realm()
        .unwrap_or_else(|| panic!("Unexpected error loading realm"))
        .clone();

    let entries = fs::read_dir(&data_dir)?
        .map(|entry| entry.map(|entry| (entry.file_name(), entry.path())))
        .collect::<Result<Vec<_>, io::Error>>()?;
    let mut character_refs = Vec::new();
    for (file_name, path) in entries {
        if let Some(object_ref) = GameObjectRef::from_file_name(&file_name) {
            let content = fs::read_to_string(&path)?;
            match hydrate(object_ref, &content) {
                Ok(object) => {
                    if let Some(character) = object.as_character() {
                        character_refs.push(character.object_ref());
                    }

                    realm = realm.set_shared_object(object_ref, (object, false));
                }
                Err(message) => println!(
                    "error loading {} {}: {}",
                    object_ref.object_type(),
                    object_ref.id(),
                    message
                ),
            }
        }
    }

    Ok(inject_characters_into_rooms(realm, character_refs))
}

// Characters get injected on load, so that rooms don't need to be re-persisted every time a
// character moves from one room to another.
fn inject_characters_into_rooms(mut realm: Realm, character_refs: Vec<GameObjectRef>) -> Realm {
    for character_ref in character_refs {
        if let Some(room) = realm
            .character(character_ref)
            .and_then(|character| realm.room(character.current_room()))
        {
            realm = realm.set(room.object_ref(), room.with_characters(vec![character_ref]));
        }
    }
    realm
}

fn process_signing_in_input(
    mut realm: Realm,
    session_tx: &Sender<SessionEvent>,
    log_tx: &LogSender,
    (input, session_id, source, state): (String, u64, String, SignInState),
) -> Realm {
    let (new_state, output) = process_input(&state, &realm, &log_tx, &source, input);
    send_session_event(session_tx, SessionEvent::SessionOutput(session_id, output));

    let session_state = if new_state.is_session_closed() {
        SessionState::SessionClosed(None)
    } else if let Some(user_name) = new_state.get_sign_in_user_name() {
        if let Some(sign_up_data) = new_state.get_sign_up_data() {
            realm = realm.with_new_player(sign_up_data);
            log_session_event(
                &log_tx,
                source.clone(),
                format!("Character created for player \"{}\"", user_name),
            );
        }

        if let Some(player) = realm.player_by_name(user_name) {
            if let Some(existing_session_id) = player.session_id() {
                send_session_event(
                    &session_tx,
                    SessionEvent::SessionUpdate(
                        existing_session_id,
                        SessionState::SessionClosed(Some(player.id())),
                    ),
                );
            }

            let player_ref = player.object_ref();
            let current_room = player.current_room();
            realm = realm.set(player_ref, player.with_session_id(Some(session_id)));

            log_command(log_tx, user_name.to_owned(), "(signed in)".to_owned());

            let mut player_output = Vec::new();
            realm = enter(realm, player_ref, current_room, &mut player_output);
            realm = look(realm, player_ref, current_room, &mut player_output);
            process_player_output(&realm, session_tx, player_output);

            SessionState::SignedIn(player_ref.id())
        } else {
            log_session_event(
                log_tx,
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

    realm
}

fn process_signed_in_input(
    realm: Realm,
    session_tx: &Sender<SessionEvent>,
    log_tx: &LogSender,
    (input, session_id, source, player_id): (String, u64, String, GameObjectId),
) -> Realm {
    if let Some(player) = realm.player_by_id(player_id) {
        log_command(&log_tx, player.name().to_owned(), input.clone());
        let player_ref = player.object_ref();
        let (new_realm, player_output) = process_player_input(realm, player_ref, log_tx, input);
        process_player_output(&new_realm, session_tx, player_output);

        new_realm
    } else {
        log_session_event(&log_tx, source, format!("Player {} deleted", player_id));
        send_session_event(
            &session_tx,
            SessionEvent::SessionUpdate(session_id, SessionState::SessionClosed(Some(player_id))),
        );
        realm
    }
}

fn process_player_input(
    realm: Realm,
    player_ref: GameObjectRef,
    _: &LogSender,
    input: String,
) -> (Realm, Vec<PlayerOutput>) {
    match interpret_command(input) {
        Ok(command) => execute_command(realm, player_ref, command),
        Err(InterpretationError::AmbiguousCommand(_)) => (
            realm,
            vec![PlayerOutput::new_from_str(
                player_ref.id(),
                "Command is not unique.\n",
            )],
        ),
        Err(InterpretationError::UnknownCommand(command)) => (
            realm,
            vec![PlayerOutput::new_from_string(
                player_ref.id(),
                format!("Command \"{}\" does not exist.\n", command),
            )],
        ),
        Err(InterpretationError::NoCommand) => (realm, vec![]),
    }
}

fn process_player_output(
    realm: &Realm,
    session_tx: &Sender<SessionEvent>,
    player_output: Vec<PlayerOutput>,
) {
    for output in player_output {
        if let Some(affected_player) = realm.player_by_id(output.player_id) {
            if let Some(session_id) = affected_player.session_id() {
                send_session_event(
                    &session_tx,
                    SessionEvent::SessionOutput(
                        session_id,
                        output.output.with(SessionOutput::Prompt(SessionPromptInfo {
                            hp: affected_player.hp(),
                            mp: affected_player.mp(),
                        })),
                    ),
                );
            }
        }
    }
}

fn process_session_closed(
    mut realm: Realm,
    _: &Sender<SessionEvent>,
    log_tx: &LogSender,
    player_id: Option<GameObjectId>,
) -> Realm {
    if let Some(player_id) = player_id {
        if let Some(player) = realm.player_by_id(player_id) {
            let player_name = player.name().to_owned();
            realm = realm.set(player.object_ref(), player.with_session_id(None));

            log_command(&log_tx, player_name, "(session closed)".to_owned());
        }
    }

    realm
}

fn send_session_event(session_tx: &Sender<SessionEvent>, event: SessionEvent) {
    if let Err(error) = session_tx.send(event) {
        println!("Could not send session event: {:?}", error);
    }
}
