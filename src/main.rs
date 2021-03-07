use std::{env, fs, io};
use tokio::sync::mpsc::{channel, Sender};

#[macro_use]
mod player_output;
#[macro_use]
mod serializable_flags;
#[macro_use]
mod game_object;

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
mod commands;
mod direction_utils;
mod events;
mod logs;
mod number_utils;
mod objects;
mod persistence_handler;
mod point3d;
mod relative_direction;
mod sessions;
mod telnet_server;
mod text_utils;
mod trigger_registry;
mod vector3d;
mod vector_utils;
mod vision_utils;
mod web_server;

use actions::{enter_room, look_at_object};
use commands::CommandExecutor;
use game_object::{
    hydrate, Character, GameObject, GameObjectId, GameObjectPersistence, GameObjectRef,
};
use logs::{log_command, log_session_event, LogMessage, LogSender};
use objects::Realm;
use persistence_handler::PersistenceRequest;
use player_output::PlayerOutput;
use sessions::{
    process_input, SessionEvent, SessionInputEvent, SessionOutput, SessionPromptInfo, SessionState,
    SignInState,
};
use trigger_registry::TriggerRegistry;

#[tokio::main]
async fn main() {
    let data_dir = env::var("PT_DATA_DIR").unwrap_or("data/".to_owned());
    let log_dir = env::var("PT_LOG_DIR").unwrap_or("logs/".to_owned());

    let telnet_port = env::var("PT_TELNET_PORT")
        .unwrap_or("".to_string())
        .parse()
        .unwrap_or(4801);
    let http_port = env::var("PT_HTTP_PORT")
        .unwrap_or("".to_string())
        .parse()
        .unwrap_or(8080);

    let mut realm = match load_data(&data_dir) {
        Ok(realm) => realm,
        Err(error) => panic!("Failed to load data from \"{}\": {}", data_dir, error),
    };

    let trigger_registry = TriggerRegistry::new();

    let (input_tx, mut input_rx) = channel::<SessionInputEvent>(64);
    let (persist_tx, persist_rx) = channel::<PersistenceRequest>(64);
    let (session_tx, session_rx) = channel::<SessionEvent>(64);
    let (log_tx, log_rx) = channel::<Box<dyn LogMessage>>(64);

    logs::create_log_handler(log_dir, log_rx);
    persistence_handler::create_persistence_handler(data_dir, persist_rx);
    sessions::create_sessions_handler(input_tx, log_tx.clone(), session_rx);

    telnet_server::serve(telnet_port, session_tx.clone());
    web_server::serve(realm.name().to_owned(), http_port, session_tx.clone());

    let command_executor = CommandExecutor::new();
    while let Some(SessionInputEvent {
        input,
        session_id,
        session_state,
        source,
    }) = input_rx.recv().await
    {
        match session_state {
            SessionState::SigningIn(state) => {
                let input_ev = (input, session_id, source, state);
                realm = process_signing_in_input(realm, &session_tx, &log_tx, input_ev).await;
            }
            SessionState::SignedIn(player_id) => {
                let input_ev = (input, session_id, source, player_id);
                realm = process_signed_in_input(
                    realm,
                    &command_executor,
                    &session_tx,
                    &trigger_registry,
                    &log_tx,
                    input_ev,
                )
                .await;
            }
            SessionState::SessionClosed(player_id) => {
                realm = process_session_closed(realm, &session_tx, &log_tx, player_id).await;
            }
        }

        let persistence_requests = realm.take_persistence_requests();
        for request in persistence_requests {
            if let Err(error) = persist_tx.send(request).await {
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

                    realm = realm.set_shared_object(
                        object_ref,
                        object,
                        GameObjectPersistence::DontSync,
                    );
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
            .map(|room| room.clone())
        {
            realm = room.add_characters(realm, vec![character_ref]);
        }
    }
    realm
}

async fn process_signing_in_input(
    mut realm: Realm,
    session_tx: &Sender<SessionEvent>,
    log_tx: &LogSender,
    (input, session_id, source, state): (String, u64, String, SignInState),
) -> Realm {
    let (new_state, output, log_messages) = process_input(&state, &realm, &source, input);

    send_session_event(session_tx, SessionEvent::SessionOutput(session_id, output)).await;
    for message in log_messages {
        log_session_event(log_tx, source.clone(), message).await;
    }

    let session_state = if new_state.is_session_closed() {
        SessionState::SessionClosed(None)
    } else if let Some(user_name) = new_state.get_sign_in_user_name() {
        if let Some(sign_up_data) = new_state.get_sign_up_data() {
            realm = realm.add_player(sign_up_data);
            log_session_event(
                &log_tx,
                source.clone(),
                format!("Character created for player \"{}\"", user_name),
            )
            .await;
        }

        if let Some(player) = realm.player_by_name(user_name).map(|player| player.clone()) {
            if let Some(existing_session_id) = player.session_id() {
                send_session_event(
                    &session_tx,
                    SessionEvent::SessionUpdate(
                        existing_session_id,
                        SessionState::SessionClosed(Some(player.id())),
                    ),
                )
                .await;
            }

            let player_ref = player.object_ref();
            let current_room = player.current_room();
            realm = player.set_session_id(realm, Some(session_id));

            log_command(log_tx, user_name.to_owned(), "(signed in)".to_owned()).await;

            let mut player_output = Vec::new();
            realm = enter_room(realm, player_ref, current_room, &mut player_output);
            look_at_object(&realm, player_ref, current_room, &mut player_output);
            process_player_output(&realm, session_tx, player_output).await;

            SessionState::SignedIn(player_ref.id())
        } else {
            log_session_event(
                log_tx,
                source,
                format!("Could not determine ID for player \"{}\"", user_name),
            )
            .await;
            SessionState::SessionClosed(None)
        }
    } else {
        SessionState::SigningIn(new_state)
    };

    send_session_event(
        &session_tx,
        SessionEvent::SessionUpdate(session_id, session_state),
    )
    .await;

    realm
}

async fn process_signed_in_input(
    realm: Realm,
    command_executor: &CommandExecutor,
    session_tx: &Sender<SessionEvent>,
    trigger_registry: &TriggerRegistry,
    log_tx: &LogSender,
    (input, session_id, source, player_id): (String, u64, String, GameObjectId),
) -> Realm {
    if let Some(player) = realm.player_by_id(player_id) {
        log_command(&log_tx, player.name().to_owned(), input.clone()).await;
        let player_ref = player.object_ref();
        let (new_realm, player_output) =
            command_executor.execute_command(realm, trigger_registry, player_ref, log_tx, input);
        process_player_output(&new_realm, session_tx, player_output).await;

        new_realm
    } else {
        log_session_event(&log_tx, source, format!("Player {} deleted", player_id)).await;
        send_session_event(
            &session_tx,
            SessionEvent::SessionUpdate(session_id, SessionState::SessionClosed(Some(player_id))),
        )
        .await;
        realm
    }
}

async fn process_player_output(
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
                        match output.output {
                            SessionOutput::JSON(json) => SessionOutput::JSON(json),
                            output => output.with(SessionOutput::Prompt(SessionPromptInfo {
                                name: affected_player.name().to_owned(),
                                is_admin: affected_player.is_admin(),
                                hp: affected_player.hp(),
                                max_hp: affected_player.stats().max_hp(),
                                mp: affected_player.mp(),
                                max_mp: affected_player.stats().max_mp(),
                            })),
                        },
                    ),
                )
                .await;
            }
        }
    }
}

async fn process_session_closed(
    mut realm: Realm,
    _: &Sender<SessionEvent>,
    log_tx: &LogSender,
    player_id: Option<GameObjectId>,
) -> Realm {
    if let Some(player_id) = player_id {
        if let Some(player) = realm.player_by_id(player_id).map(|player| player.clone()) {
            realm = player.set_session_id(realm, None);

            let player_name = player.name().to_owned();
            log_command(&log_tx, player_name, "(session closed)".to_owned()).await;
        }
    }

    realm
}

async fn send_session_event(session_tx: &Sender<SessionEvent>, event: SessionEvent) {
    if let Err(error) = session_tx.send(event).await {
        println!("Could not send session event: {:?}", error);
    }
}
