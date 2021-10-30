use super::CommandHelpers;
use crate::{
    actions::{self, ActionOutput},
    entity::{EntityRef, Realm},
};

/// Start following another character and become a group with them.
pub fn follow(realm: &mut Realm, player_ref: EntityRef, helpers: CommandHelpers) -> ActionOutput {
    let processor = helpers.command_line_processor;

    let _ = processor.take_word().unwrap(); // alias

    let (_, room) = realm.character_and_room_res(player_ref)?;
    let leader_ref = processor
        .take_entity(realm, room.characters())
        .ok_or("Follow whom?")?;

    actions::follow(realm, player_ref, leader_ref)
}
