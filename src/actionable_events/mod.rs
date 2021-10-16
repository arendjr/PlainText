use crate::{
    entity::{EntityRef, PortalFlags, Realm},
    player_output::PlayerOutput,
};

mod action_dispatcher;

pub use action_dispatcher::ActionDispatcher;

#[derive(Debug)]
pub enum ActionableEvent {
    AutoClose { entity: EntityRef, message: String },
    ResetAction { character: EntityRef },
}

impl ActionableEvent {
    pub fn process(&self, realm: &mut Realm) -> Vec<PlayerOutput> {
        match self {
            Self::AutoClose { entity, message } => process_auto_close(realm, *entity, message),
            Self::ResetAction { character } => process_reset_action(realm, *character),
        }
    }
}

fn process_auto_close(
    realm: &mut Realm,
    entity_ref: EntityRef,
    message: &str,
) -> Vec<PlayerOutput> {
    if let Some(openable) = realm.openable_mut(entity_ref) {
        if openable.is_open() {
            return Vec::new(); // Already closed.
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

    output
}

fn process_reset_action(realm: &mut Realm, character_ref: EntityRef) -> Vec<PlayerOutput> {
    if let Some(character) = realm.character_mut(character_ref) {
        character.reset_action();
    }

    Vec::new()
}
