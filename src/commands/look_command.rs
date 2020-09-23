use crate::actions::{look_at_object, look_in_direction};
use crate::direction_utils::{direction_by_abbreviation, is_direction, vector_for_direction};
use crate::game_object::{Character, GameObjectRef};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::relative_direction::RelativeDirection;

use super::CommandLineProcessor;

/// Makes the character look at *something*.
///
/// Examples: `look`, `look at door`, `look earl`, `examine sign`, `look at key in inventory`
pub fn look(
    realm: Realm,
    player_ref: GameObjectRef,
    mut processor: CommandLineProcessor,
) -> (Realm, Vec<PlayerOutput>) {
    let mut output = Vec::new();
    let player = unwrap_or_return!(realm.player(player_ref), (realm, output));
    let current_room = unwrap_or_return!(realm.room(player.current_room()), (realm, output));

    let alias = processor.take_word().unwrap();

    if alias.starts_with('l') {
        if !processor.has_words_left() {
            look_at_object(&realm, player_ref, player.current_room(), &mut output);
            return (realm, output);
        }

        processor.skip_connecting_word("at");
    }

    if processor.peek_rest() == "inventory" || processor.peek_rest() == "in inventory" {
        /* TODO: execute inventory action
        player.execute("inventory");
        return;*/
    }

    let description = match processor.take_object_description() {
        Some(description) => description,
        None => {
            if alias == "examine" {
                push_output_str!(output, player_ref, "Examine what?\n");
            } else {
                push_output_str!(output, player_ref, "Look at what?\n");
            }
            return (realm, output);
        }
    };

    if processor.peek_rest() == "in inventory" {
        if let Some(object_ref) =
            processor.object_by_description(&realm, player.inventory(), description)
        {
            look_at_object(&realm, player_ref, object_ref, &mut output);
        } else {
            push_output_str!(output, player_ref, "You don't have that.\n");
        }
        return (realm, output);
    }

    let pool = [
        &current_room.characters()[..],
        &current_room.items()[..],
        &current_room.portals()[..],
        &player.inventory()[..],
    ]
    .concat();

    let maybe_object = processor
        .object_by_description(&realm, &pool, description.clone())
        .and_then(|object_ref| realm.object(object_ref));
    let object = match maybe_object {
        Some(object) => object,
        None => {
            let mut target = description.name.as_ref();
            if let Some(direction) = direction_by_abbreviation(target) {
                target = direction;
            }
            let direction = if is_direction(target) {
                push_output_string!(output, player_ref, format!("You look {}.\n", target));
                vector_for_direction(target)
            } else if target == "left" {
                push_output_str!(output, player_ref, "You look to the left.\n");
                RelativeDirection::Left.from(player.direction())
            } else if target == "right" {
                push_output_str!(output, player_ref, "You look to the right.\n");
                RelativeDirection::Right.from(player.direction())
            } else if target == "back" || target == "behind" {
                push_output_str!(output, player_ref, "You look behind you.\n");
                RelativeDirection::Behind.from(player.direction())
            } else if target == "up" {
                push_output_str!(output, player_ref, "You look up.\n");
                RelativeDirection::Up.from(player.direction())
            } else if target == "down" {
                push_output_str!(output, player_ref, "You look down.\n");
                RelativeDirection::Down.from(player.direction())
            } else {
                if alias == "examine" {
                    push_output_str!(output, player_ref, "Examine what?\n");
                } else {
                    push_output_str!(output, player_ref, "Look where?\n");
                }
                return (realm, output);
            };

            look_in_direction(&realm, player_ref, &direction, &mut output);
            return (realm, output);
        }
    };

    look_at_object(&realm, player_ref, object.object_ref(), &mut output);

    /*
    TODO: There is more...
    function describeNearbyObject(nearbyObject, vector, position) {
        if (vector.angle(position) < Math.PI / 8) {
            var angle = Util.angleBetweenXYVectors(vector, position);

            if (nearbyObject.isPortal()) {
                player.send("On its %1 is %2.".arg(angle > 0 ? "right" : "left",
                                                   nearbyObject.nameWithDestinationFromRoom(room)));
            } else {
                player.send("On its %1 there's %2.".arg(angle > 0 ? "right" : "left",
                                                        nearbyObject.indefiniteName()));
            }
        }
    }

    var showNearbyObjects = (object.isItem() && !object.position.equals([0, 0, 0])) ||
                            object.isPortal();
    if (showNearbyObjects) {
        var vector = (object.isPortal() ? object.position.minus(room.position) : object.position);
        room.items.forEach(function(item) {
            if (item !== object) {
                describeNearbyObject(item, vector, item.position);
            }
        });
        room.portals.forEach(function(portal) {
            if (portal !== object) {
                describeNearbyObject(portal, vector, portal.position.minus(room.position));
            }
        });
    }

    if (object.isPortal() && object.canSeeThrough()) {
        var strength = room.eventMultiplier("Visual");
        if (player.weapon && player.weapon.name === "binocular") {
            strength *= 4;
        }

        var characters = VisualUtil.charactersVisibleThroughPortal(player, room, object, strength);
        var characterText = VisualUtil.describeCharactersRelativeTo(characters, player);
        if (characterText !== "") {
            player.send(characterText);
        }
    }
    */

    (realm, output)
}
