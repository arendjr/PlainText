use super::CommandHelpers;
use crate::{
    actions,
    game_object::{Character, GameObject, GameObjectRef},
    objects::Realm,
    player_output::PlayerOutput,
    relative_direction::RelativeDirection,
    text_utils::{describe_objects_from_room, join_sentence},
    vision_utils::visible_portals_from_position,
};

/// Makes the character travel to another room.
pub fn go(
    realm: &mut Realm,
    player_ref: GameObjectRef,
    helpers: CommandHelpers,
) -> Result<Vec<PlayerOutput>, String> {
    let processor = helpers.command_line_processor;

    let alias = processor.take_word().unwrap();

    if alias == "go" {
        if let Some(relative_direction) = processor
            .peek_word()
            .and_then(RelativeDirection::from_string)
        {
            return go_in_direction(realm, player_ref, relative_direction);
        }

        processor.skip_connecting_word("to");
    }

    let (_, room) = realm.player_and_room_res(player_ref)?;

    let portal_ref = processor
        .take_object(realm, room.portals())
        .ok_or("Go where?")?;

    let room_ref = room.object_ref();
    actions::enter_portal(realm, player_ref, portal_ref, room_ref)
}

pub fn go_in_direction(
    realm: &mut Realm,
    player_ref: GameObjectRef,
    relative_direction: RelativeDirection,
) -> Result<Vec<PlayerOutput>, String> {
    let (player, room) = realm.player_and_room_res(player_ref)?;

    let direction = &relative_direction.from(player.direction());
    let portal_refs = visible_portals_from_position(realm, room, direction);
    if portal_refs.is_empty() {
        Err(format!("There's no way {}.", direction))
    } else if let Some(portal_ref) = GameObjectRef::only(&portal_refs) {
        let room_ref = room.object_ref();
        actions::enter_portal(realm, player_ref, portal_ref, room_ref)
    } else {
        let destination_descriptions =
            describe_objects_from_room(realm, &portal_refs, room.object_ref());
        Err(format!(
            "There are multiple ways {}, to the {}.",
            direction,
            join_sentence(&destination_descriptions)
        ))
    }
}
