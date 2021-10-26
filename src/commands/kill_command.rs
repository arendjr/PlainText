use super::CommandHelpers;
use crate::{
    actions,
    entity::{EntityRef, Realm},
    player_output::PlayerOutput,
};

/// Attacks another character.
///
/// Examples: `kill joe`, `attack earl`
pub fn kill(
    realm: &mut Realm,
    player_ref: EntityRef,
    helpers: CommandHelpers,
) -> Result<Vec<PlayerOutput>, String> {
    let processor = helpers.command_line_processor;

    let _ = processor.take_word().unwrap(); // alias

    let (_, room) = realm.character_and_room_res(player_ref)?;
    let character_ref = processor
        .take_entity(realm, room.characters())
        .ok_or("Kill who?")?;

    actions::kill(realm, helpers.action_dispatcher, player_ref, character_ref)
}
