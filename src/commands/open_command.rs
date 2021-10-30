use super::CommandHelpers;
use crate::{
    actions::{self, ActionOutput},
    entity::{EntityRef, Realm},
};

/// Opens an exit.
///
/// Example: `open door`
pub fn open(realm: &mut Realm, player_ref: EntityRef, helpers: CommandHelpers) -> ActionOutput {
    let processor = helpers.command_line_processor;

    let _ = processor.take_word().unwrap(); // alias

    let (_, room) = realm.character_and_room_res(player_ref)?;
    let portal_ref = processor
        .take_entity(realm, room.portals())
        .ok_or("Open what?")?;

    actions::open(realm, helpers.action_dispatcher, player_ref, portal_ref)
}
