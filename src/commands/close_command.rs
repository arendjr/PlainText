use super::CommandHelpers;
use crate::{actions, game_object::GameObjectRef, objects::Realm, player_output::PlayerOutput};

/// Closes an exit.
///
/// Example: `close door`
pub fn close(
    realm: &mut Realm,
    player_ref: GameObjectRef,
    helpers: CommandHelpers,
) -> Result<Vec<PlayerOutput>, String> {
    let processor = helpers.command_line_processor;

    let _ = processor.take_word().unwrap(); // alias

    let (_, room) = realm.character_and_room_res(player_ref)?;
    let portal_ref = processor
        .take_object(realm, room.portals())
        .ok_or("Close what?")?;

    actions::close(realm, player_ref, portal_ref)
}
