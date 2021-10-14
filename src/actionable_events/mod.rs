use crate::{
    entity::{EntityRef, PortalFlags, Realm},
    player_output::PlayerOutput,
};
use std::time::Duration;
use tokio::{sync::mpsc::Sender, time::sleep};

pub struct ActionDispatcher {
    action_tx: Sender<ActionableEvent>,
}

impl ActionDispatcher {
    pub fn new(action_tx: Sender<ActionableEvent>) -> Self {
        Self { action_tx }
    }

    pub fn dispatch_after(&self, action: ActionableEvent, delay: Duration) {
        let action_tx = self.action_tx.clone();
        tokio::spawn(async move {
            sleep(delay).await;

            action_tx
                .send(action)
                .await
                .expect("Failed to dispatch action");
        });
    }
}

#[derive(Debug)]
pub enum ActionableEvent {
    AutoClose {
        entity_ref: EntityRef,
        message: String,
    },
}

impl ActionableEvent {
    pub fn process(&self, realm: &mut Realm) -> Vec<PlayerOutput> {
        match self {
            Self::AutoClose {
                entity_ref,
                message,
            } => process_auto_close(realm, *entity_ref, message),
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
