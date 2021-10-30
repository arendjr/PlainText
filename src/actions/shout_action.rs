use super::ActionOutput;
use crate::{
    entity::{EntityRef, Realm},
    events::SpeechEvent,
};

/// Shouts something.
pub fn shout(realm: &mut Realm, character_ref: EntityRef, message: &str) -> ActionOutput {
    let mut event = SpeechEvent::new(character_ref, message.to_owned());
    let output = event.fire(realm, 5.0);

    /* TODO: var self = this;
    event.affectedCharacters.forEach(function(character) {
        character.invokeTrigger("onshout", self, message);
    });*/

    output.ok_or_else(|| "You shouted, but nobody heard you.".into())
}
