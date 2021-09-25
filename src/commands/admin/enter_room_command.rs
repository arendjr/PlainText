use crate::actions;
use crate::game_object::{GameObjectRef, GameObjectType};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;

use super::CommandHelpers;

/// Enters a room without the need for there to be an exit to the room.
pub fn enter_room(
    realm: &mut Realm,
    player_ref: GameObjectRef,
    helpers: CommandHelpers,
) -> Result<Vec<PlayerOutput>, String> {
    let processor = helpers.command_line_processor;
    processor.take_word(); // alias

    let room_id_str = processor.take_word().ok_or("Enter what room?")?;
    let room_id = room_id_str
        .strip_prefix('#')
        .and_then(|room_id| room_id.parse::<u32>().ok())
        .ok_or("Provide a room ID preceded by #.")?;
    let room_ref = GameObjectRef::new(GameObjectType::Room, room_id);

    actions::enter_room(realm, player_ref, room_ref)
}
