use super::CommandHelpers;
use crate::{
    actions::{self, ActionOutput},
    entity::{EntityRef, Realm},
};

/// Makes the character shout something.
pub fn shout(realm: &mut Realm, player_ref: EntityRef, helpers: CommandHelpers) -> ActionOutput {
    let processor = helpers.command_line_processor;

    processor.take_word().unwrap(); // alias
    if !processor.has_words_left() {
        return Err("Shout what?".into());
    }

    let message = processor.take_rest();
    actions::shout(realm, player_ref, &message)
}
