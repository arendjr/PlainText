use super::CommandHelpers;
use crate::{
    actions::{self, ActionOutput},
    entity::{EntityRef, Realm},
};

/// Attacks another character.
///
/// Examples: `kill joe`, `attack earl`
pub fn kill(realm: &mut Realm, player_ref: EntityRef, helpers: CommandHelpers) -> ActionOutput {
    let processor = helpers.command_line_processor;

    let _ = processor.take_word().unwrap(); // alias

    let (_, room) = realm.character_and_room_res(player_ref)?;
    let character_ref = processor
        .take_entity(realm, room.characters())
        .ok_or("Kill who?")?;

    actions::kill(realm, helpers.action_dispatcher, player_ref, character_ref)
}
