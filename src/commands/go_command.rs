use super::CommandHelpers;
use crate::{
    actions::{self, ActionOutput},
    entity::{Entity, EntityRef, Realm},
    relative_direction::RelativeDirection,
    utils::{describe_entities_from_room, join_sentence, visible_portals_from_position},
};

/// Makes the character travel to another room.
pub fn go(realm: &mut Realm, player_ref: EntityRef, helpers: CommandHelpers) -> ActionOutput {
    let processor = helpers.command_line_processor;

    let alias = processor.take_word().unwrap();

    let relative_direction = if alias == "go" {
        let relative_direction = processor
            .peek_word()
            .and_then(RelativeDirection::from_string);

        processor.skip_connecting_word("to");

        relative_direction
    } else {
        None
    };

    let (_, room) = realm.character_and_room_res(player_ref)?;

    let portal_ref = match processor.take_entity(realm, room.portals()) {
        Some(portal_ref) => Ok(portal_ref),
        None => {
            if let Some(direction) = relative_direction {
                get_portal_in_direction(realm, player_ref, direction)
            } else {
                Err("Go where?".into())
            }
        }
    }?;

    let room_ref = room.entity_ref();
    actions::enter_portal(
        realm,
        helpers.action_dispatcher,
        player_ref,
        portal_ref,
        room_ref,
    )
}

pub fn get_portal_in_direction(
    realm: &Realm,
    player_ref: EntityRef,
    relative_direction: RelativeDirection,
) -> Result<EntityRef, String> {
    let (player, room) = realm.character_and_room_res(player_ref)?;

    let direction = &relative_direction.from(player.direction());
    let portal_refs = visible_portals_from_position(realm, room, direction);
    if portal_refs.is_empty() {
        Err(format!("There's no way {}.", relative_direction))
    } else if let Some(portal_ref) = EntityRef::only(&portal_refs) {
        Ok(portal_ref)
    } else {
        let destination_descriptions =
            describe_entities_from_room(realm, &portal_refs, room.entity_ref());
        Err(format!(
            "There are multiple ways {}, to the {}.",
            relative_direction,
            join_sentence(&destination_descriptions)
        ))
    }
}
