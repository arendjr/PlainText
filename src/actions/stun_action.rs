use crate::{
    actionable_events::ActionDispatcher,
    actions,
    entity::{CharacterAction, EntityRef, Realm},
};
use std::time::{Duration, SystemTime};

/// Stuns the character for the specified duration.
pub fn stun(
    realm: &mut Realm,
    action_dispatcher: &ActionDispatcher,
    character_ref: EntityRef,
    duration: Duration,
) {
    let action = CharacterAction::Stunned {
        end_time: SystemTime::now() + duration,
    };
    actions::set_character_action(realm, action_dispatcher, character_ref, action, duration);
}
