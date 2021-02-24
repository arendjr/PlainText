use crate::actions::{enter_portal, look_at_object};
use crate::game_object::{Character, GameObject, GameObjectRef};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::relative_direction::RelativeDirection;
use crate::text_utils::{describe_objects_from_room, join_sentence};
use crate::vision_utils::visible_portals_from_position;

use super::CommandHelpers;

/// Makes the character travel to another room.
///
/// Examples: `go north`, `go to tower`, `enter door`, `go forward`
pub fn go(
    mut realm: Realm,
    player_ref: GameObjectRef,
    helpers: CommandHelpers,
) -> (Realm, Vec<PlayerOutput>) {
    let processor = helpers.command_line_processor;

    let mut output = Vec::new();

    let alias = processor.take_word().unwrap();

    if alias == "go" {
        if let Some(relative_direction) = processor
            .peek_word()
            .and_then(RelativeDirection::from_string)
        {
            realm = go_in_direction(realm, player_ref, relative_direction, &mut output);
            return (realm, output);
        }

        processor.skip_connecting_word("to");
    }

    let player = unwrap_or_return!(realm.player(player_ref), (realm, output));
    let current_room = unwrap_or_return!(realm.room(player.current_room()), (realm, output));
    let maybe_portal_ref = processor.take_object(&realm, current_room.portals());

    if let Some(portal_ref) = maybe_portal_ref {
        let current_room_ref = current_room.object_ref();
        realm = enter_portal(realm, player_ref, portal_ref, current_room_ref, &mut output);
        if let Some(player) = realm.player(player_ref) {
            look_at_object(&realm, player_ref, player.current_room(), &mut output);
        }
    } else {
        push_output_str!(output, player_ref, "Go where?\n");
    }

    (realm, output)
}

pub fn go_in_direction(
    mut realm: Realm,
    player_ref: GameObjectRef,
    relative_direction: RelativeDirection,
    output: &mut Vec<PlayerOutput>,
) -> Realm {
    let player = unwrap_or_return!(realm.player(player_ref), realm);
    let current_room = unwrap_or_return!(realm.room(player.current_room()), realm);
    let current_room_ref = current_room.object_ref();

    let direction = &relative_direction.from(player.direction());
    let portal_refs = visible_portals_from_position(&realm, current_room, direction);
    if portal_refs.is_empty() {
        push_output_string!(
            output,
            player_ref,
            format!("There's no way {}.\n", direction)
        );
    } else if let Some(portal_ref) = GameObjectRef::only(&portal_refs) {
        realm = enter_portal(realm, player_ref, portal_ref, current_room_ref, output);
    } else {
        let destination_descriptions =
            describe_objects_from_room(&realm, &portal_refs, current_room_ref);
        push_output_string!(
            output,
            player_ref,
            format!(
                "There are multiple ways {}, to the {}.\n",
                direction,
                join_sentence(destination_descriptions)
            )
        );
    }
    realm
}
