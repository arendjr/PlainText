#![allow(non_upper_case_globals)]

use actionable_events::ActionDispatcher;
use std::{collections::BTreeMap, env, fs, io};
use tokio::sync::mpsc::{channel, Sender};
use utils::total_stats;

#[macro_use]
mod player_output;
#[macro_use]
mod serializable_flags;
#[macro_use]
mod entity;

mod actionable_events;
mod actions;
mod character_stats;
mod colors;
mod commands;
mod events;
mod logs;
mod persistence_handler;
mod point3d;
mod relative_direction;
mod sessions;
mod telnet_server;
mod utils;
mod vector3d;
mod web_server;

use actions::{enter_room, look_at_entity};
use commands::CommandExecutor;
use entity::{hydrate, Entity, EntityId, EntityRef, EntityType, Realm};
use logs::{log_command, log_session_event, LogMessage, LogSender};
use persistence_handler::PersistenceRequest;
use player_output::PlayerOutput;
use sessions::{
    process_input, ProcessSignInInputResult, SessionEvent, SessionInputEvent, SessionOutput,
    SessionPromptInfo, SessionState, SignInState,
};

use crate::{actionable_events::ActionableEvent, logs::log_error};

#[tokio::main]
async fn main() {
    let data_dir = env::var("PT_DATA_DIR").unwrap_or_else(|_| "data/".to_owned());
    let log_dir = env::var("PT_LOG_DIR").unwrap_or_else(|_| "logs/".to_owned());

    let telnet_port = env::var("PT_TELNET_PORT")
        .unwrap_or_else(|_| "".to_string())
        .parse()
        .unwrap_or(4801);
    let http_port = env::var("PT_HTTP_PORT")
        .unwrap_or_else(|_| "".to_string())
        .parse()
        .unwrap_or(8080);

    let (mut realm, character_refs) = match load_data(&data_dir) {
        Ok(realm) => realm,
        Err(error) => panic!("Failed to load data from \"{}\": {}", data_dir, error),
    };

    let (action_tx, mut action_rx) = channel::<ActionableEvent>(64);
    let (input_tx, mut input_rx) = channel::<SessionInputEvent>(64);
    let (persist_tx, persist_rx) = channel::<PersistenceRequest>(64);
    let (session_tx, session_rx) = channel::<SessionEvent>(64);
    let (log_tx, log_rx) = channel::<Box<dyn LogMessage>>(64);

    logs::create_log_handler(log_dir, log_rx);
    persistence_handler::create_persistence_handler(data_dir, persist_rx);
    sessions::create_sessions_handler(input_tx, log_tx.clone(), session_rx);

    telnet_server::serve(telnet_port, session_tx.clone());
    web_server::serve(realm.name().to_owned(), http_port, session_tx.clone());

    let action_dispatcher = ActionDispatcher::new(action_tx.clone());
    for character_ref in character_refs {
        if let Some(actor) = realm.actor(character_ref) {
            if let Err(error) = actor.borrow().on_spawn(&mut realm, &action_dispatcher) {
                println!("Error spawning character {}: {}", character_ref, error);
            }
        }
    }

    let command_executor = CommandExecutor::new(action_tx.clone());
    loop {
        tokio::select! {
            Some(SessionInputEvent {
                input,
                session_id,
                session_state,
                source,
            }) = input_rx.recv() => {
                match session_state {
                    SessionState::SigningIn(state) => {
                        let input_ev = (input, session_id, source, state.as_ref());
                        process_signing_in_input(
                            &mut realm,
                            &action_dispatcher,
                            &session_tx,
                            &log_tx,
                            input_ev
                        )
                        .await;
                    }
                    SessionState::SignedIn(player_id) => {
                        let input_ev = (input, session_id, source, player_id);
                        process_signed_in_input(
                            &mut realm,
                            &command_executor,
                            &session_tx,
                            &log_tx,
                            input_ev,
                        )
                        .await;
                    }
                    SessionState::SessionClosed(player_id) => {
                        process_session_closed(&mut realm, &session_tx, &log_tx, player_id).await;
                    }
                }
            }
            Some(event) = action_rx.recv() => {
                match event.process(&mut realm, &action_dispatcher) {
                    Ok(player_output) =>
                        process_player_output(&realm, &session_tx, player_output).await,
                    Err(message) =>
                        log_error(
                            &log_tx,
                            format!("Error processing event {:?}: {}", event, message),
                        ).await
                }
            }
            else => {
                break;
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

fn load_data(data_dir: &str) -> Result<(Realm, Vec<EntityRef>), io::Error> {
    let content = fs::read_to_string(&format!("{}/realm.000000000", data_dir))?;
    let mut realm = Realm::hydrate(0, &content)
        .unwrap_or_else(|message| panic!("Failed to load realm: {}", message));

    let entries = fs::read_dir(&data_dir)?
        .map(|entry| entry.map(|entry| (entry.file_name(), entry.path())))
        .collect::<Result<Vec<_>, io::Error>>()?;
    let mut character_refs = Vec::new();
    for (file_name, path) in entries {
        if let Some(entity_ref) = EntityRef::from_file_name(&file_name) {
            let content = fs::read_to_string(&path)?;
            match hydrate(entity_ref, &content) {
                Ok(entity) => {
                    if entity.as_npc().is_some() {
                        character_refs.push(entity_ref);
                    }

                    realm.set(entity_ref, entity);
                }
                Err(message) => println!(
                    "error loading {} {}: {}",
                    entity_ref.entity_type(),
                    entity_ref.id(),
                    message
                ),
            }
        }
    }

    inject_characters_into_rooms(&mut realm, &character_refs);
    Ok((realm, character_refs))
}

// Characters get injected on load, so that rooms don't need to be re-persisted every time a
// character moves from one room to another.
fn inject_characters_into_rooms(realm: &mut Realm, character_refs: &[EntityRef]) {
    for character_ref in character_refs {
        let maybe_current_room = realm
            .character(*character_ref)
            .map(|character| character.current_room());
        if let Some(room) = maybe_current_room.and_then(|room_ref| realm.room_mut(room_ref)) {
            room.add_characters(&[*character_ref]);
        } else {
            println!(
                "Character {:?} has no room {:?}",
                character_ref, maybe_current_room
            );
        }
    }
}

async fn process_signing_in_input(
    realm: &mut Realm,
    action_dispatcher: &ActionDispatcher,
    session_tx: &Sender<SessionEvent>,
    log_tx: &LogSender,
    (input, session_id, source, state): (String, u64, String, &SignInState),
) {
    let ProcessSignInInputResult {
        new_state,
        output,
        log_messages,
    } = process_input(state, realm, &source, input);

    send_session_event(session_tx, SessionEvent::SessionOutput(session_id, output)).await;
    for message in log_messages {
        log_session_event(log_tx, source.clone(), message).await;
    }

    let session_state = if new_state.is_session_closed() {
        SessionState::SessionClosed(None)
    } else if let Some(user_name) = new_state.get_sign_in_user_name() {
        if let Some(sign_up_data) = new_state.get_sign_up_data() {
            realm.add_player(sign_up_data);
            log_session_event(
                log_tx,
                source.clone(),
                format!("Character created for player \"{}\"", user_name),
            )
            .await;
        }

        if let Some(player) = realm.player_by_name_mut(user_name) {
            if let Some(existing_session_id) = player.session_id() {
                send_session_event(
                    session_tx,
                    SessionEvent::SessionUpdate(
                        existing_session_id,
                        SessionState::SessionClosed(Some(player.id())),
                    ),
                )
                .await;
            }

            let player_ref = player.entity_ref();
            let current_room = player.character.current_room();
            player.set_session_id(Some(session_id));

            log_command(log_tx, user_name.to_owned(), "(signed in)".to_owned()).await;

            let output = match enter_room(realm, action_dispatcher, player_ref, current_room) {
                Ok(mut output) => {
                    if let Ok(mut more) = look_at_entity(realm, player_ref, current_room) {
                        output.append(&mut more);
                    }
                    output
                }
                Err(message) => vec![PlayerOutput::new_from_string(player_ref.id(), message)],
            };
            process_player_output(realm, session_tx, output).await;

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
        SessionState::SigningIn(Box::new(new_state))
    };

    send_session_event(
        session_tx,
        SessionEvent::SessionUpdate(session_id, session_state),
    )
    .await;
}

async fn process_signed_in_input(
    realm: &mut Realm,
    command_executor: &CommandExecutor,
    session_tx: &Sender<SessionEvent>,
    log_tx: &LogSender,
    (input, session_id, source, player_id): (String, u64, String, EntityId),
) {
    if let Some(player) = realm.player_by_id(player_id) {
        log_command(log_tx, player.name().to_owned(), input.clone()).await;
        let player_ref = player.entity_ref();
        let player_output = command_executor.execute_command(realm, player_ref, log_tx, input);
        process_player_output(realm, session_tx, player_output).await;
    } else {
        log_session_event(log_tx, source, format!("Player {} deleted", player_id)).await;
        send_session_event(
            session_tx,
            SessionEvent::SessionUpdate(session_id, SessionState::SessionClosed(Some(player_id))),
        )
        .await;
    }
}

async fn process_player_output(
    realm: &Realm,
    session_tx: &Sender<SessionEvent>,
    player_output: Vec<PlayerOutput>,
) {
    let mut aggregated_output = BTreeMap::<u32, SessionOutput>::new();
    for PlayerOutput { player_id, output } in player_output {
        if let Some(existing_output) = aggregated_output.remove(&player_id) {
            aggregated_output.insert(player_id, existing_output.with(output));
        } else {
            aggregated_output.insert(player_id, output);
        }
    }

    for (player_id, output) in aggregated_output {
        let player = match realm.player_by_id(player_id) {
            Some(player) => player,
            None => continue,
        };

        if let Some(session_id) = player.session_id() {
            send_session_event(
                session_tx,
                SessionEvent::SessionOutput(
                    session_id,
                    match output {
                        SessionOutput::Json(json) => SessionOutput::Json(json),
                        output => {
                            let stats = total_stats(realm, player.entity_ref());
                            output.with(SessionOutput::Prompt(SessionPromptInfo {
                                name: player.name().to_owned(),
                                is_admin: player.is_admin(),
                                hp: player.character.hp(),
                                max_hp: stats.max_hp(),
                                mp: player.character.mp(),
                                max_mp: stats.max_mp(),
                            }))
                        }
                    },
                ),
            )
            .await;
        }
    }
}

async fn process_session_closed(
    realm: &mut Realm,
    session_tx: &Sender<SessionEvent>,
    log_tx: &LogSender,
    player_id: Option<EntityId>,
) {
    if let Some(player_id) = player_id {
        let player_ref = EntityRef::new(EntityType::Player, player_id);
        let mut should_unfollow = false;

        if let Some(player) = realm.player_by_id_mut(player_id) {
            player.set_session_id(None);

            should_unfollow = player.character.group().is_some();

            let player_name = player.name().to_owned();
            log_command(log_tx, player_name, "(session closed)".to_owned()).await;
        }

        let mut output = vec![];

        if should_unfollow {
            if let Ok(mut player_output) = actions::unfollow(realm, player_ref) {
                output.append(&mut player_output);
            }
        }

        if let Ok(mut player_output) = actions::leave_room(realm, player_ref) {
            output.append(&mut player_output);
        }

        process_player_output(realm, session_tx, output).await;
    }
}

async fn send_session_event(session_tx: &Sender<SessionEvent>, event: SessionEvent) {
    if let Err(error) = session_tx.send(event).await {
        println!("Could not send session event: {:?}", error);
    }
}
