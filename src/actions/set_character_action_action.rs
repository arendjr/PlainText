use crate::{
    actionable_events::{ActionDispatcher, ActionableEvent},
    entity::{CharacterAction, EntityRef, Realm},
};
use std::time::Duration;

/// Sets the action the given character is occupied with for the given duration.
pub fn set_character_action(
    realm: &mut Realm,
    action_dispatcher: &ActionDispatcher,
    character_ref: EntityRef,
    action: CharacterAction,
    duration: Duration,
) {
    if let Some(character) = realm.character_mut(character_ref) {
        let reset_abort_handle = action_dispatcher
            .dispatch_cancelable_after(ActionableEvent::ResetAction(character_ref), duration);

        character.set_action(action, reset_abort_handle);
    }
}
