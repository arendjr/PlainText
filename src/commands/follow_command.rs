use super::CommandHelpers;
use crate::{actions, game_object::GameObjectRef, objects::Realm, player_output::PlayerOutput};

/// Start following another character and become a group with them.
pub fn follow(
    realm: &mut Realm,
    player_ref: GameObjectRef,
    helpers: CommandHelpers,
) -> Result<Vec<PlayerOutput>, String> {
    let processor = helpers.command_line_processor;

    let _ = processor.take_word().unwrap(); // alias

    let (_, room) = realm.player_and_room_res(player_ref)?;
    let leader_ref = processor
        .take_object(realm, room.characters())
        .ok_or("Follow whom?")?;

    actions::follow(realm, player_ref, leader_ref)
}
