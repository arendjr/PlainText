use super::CommandHelpers;
use crate::{
    actions,
    entity::{EntityRef, EntityType, Realm},
    player_output::PlayerOutput,
};

/// Enters a room without the need for there to be an exit to the room.
pub fn enter_room(
    realm: &mut Realm,
    player_ref: EntityRef,
    helpers: CommandHelpers,
) -> Result<Vec<PlayerOutput>, String> {
    let processor = helpers.command_line_processor;
    processor.take_word(); // alias

    let room_id_str = processor.take_word().ok_or("Enter what room?")?;
    let room_id = room_id_str
        .strip_prefix('#')
        .and_then(|room_id| room_id.parse::<u32>().ok())
        .ok_or("Provide a room ID preceded by #.")?;
    let room_ref = EntityRef::new(EntityType::Room, room_id);

    actions::enter_room(realm, helpers.action_dispatcher, player_ref, room_ref)
}
