use crate::{
    actions::{self, ActionOutput},
    entity::{CharacterAction, EntityRef, PortalFlags, Realm},
    player_output::PlayerOutput,
    utils::total_stats,
};
pub use action_dispatcher::ActionDispatcher;

mod action_dispatcher;

#[derive(Debug)]
pub enum ActionableEvent {
    ActivateActor(EntityRef),
    AutoClose(EntityRef, String),
    ResetAction(EntityRef),
    SpawnNpc(EntityRef),
}

impl ActionableEvent {
    pub fn process(
        &self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
    ) -> Result<Vec<PlayerOutput>, String> {
        match self {
            Self::ActivateActor(entity) => process_activate_actor(realm, dispatcher, *entity),
            Self::AutoClose(entity, message) => process_auto_close(realm, *entity, message),
            Self::ResetAction(character) => process_reset_action(realm, dispatcher, *character),
            Self::SpawnNpc(character) => process_spawn_npc(realm, dispatcher, *character),
        }
    }
}

fn process_activate_actor(
    realm: &mut Realm,
    dispatcher: &ActionDispatcher,
    entity_ref: EntityRef,
) -> Result<Vec<PlayerOutput>, String> {
    if let Some(actor) = realm.actor(entity_ref) {
        actor.borrow().on_active(realm, dispatcher)
    } else {
        Err(format!("Actor doesn't exist: {}", entity_ref))
    }
}

fn process_auto_close(
    realm: &mut Realm,
    entity_ref: EntityRef,
    message: &str,
) -> Result<Vec<PlayerOutput>, String> {
    if let Some(openable) = realm.openable_mut(entity_ref) {
        if openable.is_open() {
            return Ok(Vec::new()); // Already closed.
        }

        openable.set_open(false);
    }

    let mut output = Vec::new();

    if let Some(portal) = realm.portal(entity_ref) {
        let message = format!("{}\n", message);
        if portal.has_flags(PortalFlags::CanOpenFromSide1) {
            if let Some(room) = realm.room(portal.room()) {
                for character in room.characters() {
                    output.push(PlayerOutput::new_from_string(
                        character.id(),
                        message.clone(),
                    ));
                }
            }
        }
        if portal.has_flags(PortalFlags::CanOpenFromSide2) {
            if let Some(room) = realm.room(portal.room2()) {
                for character in room.characters() {
                    output.push(PlayerOutput::new_from_string(
                        character.id(),
                        message.clone(),
                    ));
                }
            }
        }
    }

    Ok(output)
}

fn process_reset_action(
    realm: &mut Realm,
    dispatcher: &ActionDispatcher,
    character_ref: EntityRef,
) -> ActionOutput {
    if let Some(character) = realm.character_mut(character_ref) {
        character.set_indefinite_action(CharacterAction::Idle);
    }

    if let Some(actor) = realm.actor(character_ref) {
        actor.borrow().on_active(realm, dispatcher)
    } else {
        Ok(Vec::new())
    }
}

fn process_spawn_npc(
    realm: &mut Realm,
    dispatcher: &ActionDispatcher,
    character_ref: EntityRef,
) -> ActionOutput {
    let stats = total_stats(realm, character_ref);
    if let Some(character) = realm.character_mut(character_ref) {
        character.set_hp(stats.max_hp());
        character.set_mp(stats.max_mp());
    }

    let mut output = actions::enter_current_room(realm, character_ref)?;

    if let Some(actor) = realm.actor(character_ref) {
        output.append(&mut actor.borrow().on_spawn(realm, dispatcher)?);
    }

    Ok(output)
}
