use super::CommandHelpers;
use crate::{
    actions::{self, ActionOutput},
    entity::{Entity, EntityRef, Realm},
};

/// Remove yourself or someone else from a group. If you are a group
/// leader, you can remove anyone from your group by using *lose <name>*.
/// You can always remove yourself from a group using simply *lose*.
///
/// Examples: `lose mia`, `lose`
pub fn lose(realm: &mut Realm, player_ref: EntityRef, helpers: CommandHelpers) -> ActionOutput {
    let processor = helpers.command_line_processor;

    let _ = processor.take_word().unwrap(); // alias

    let (player, room) = realm.character_and_room_res(player_ref)?;
    let group = player
        .group()
        .and_then(|group| realm.group(group))
        .ok_or("You're not in any group.")?;
    let group_ref = group.entity_ref();

    if processor.has_words_left() {
        if player_ref != group.leader() {
            return Err("Only the group leader can lose people from the group.".into());
        }

        if processor.peek_word() == Some("all") {
            return actions::disband(realm, group_ref);
        }

        match processor.take_entity(realm, room.characters()) {
            Some(character) => actions::lose(realm, character),
            None => Err("Lose whom?".into()),
        }
    } else if player_ref == group.leader() {
        actions::disband(realm, group_ref)
    } else {
        actions::unfollow(realm, player_ref)
    }
}
