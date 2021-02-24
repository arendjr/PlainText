use crate::actions::{look_at_object, look_in_direction};
use crate::direction_utils::{direction_by_abbreviation, is_direction, vector_for_direction};
use crate::game_object::{Character, GameObjectRef};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::relative_direction::RelativeDirection;

use super::inventory_command::inventory;
use super::{CommandHelpers, CommandLineProcessor};

/// Makes the character look at *something*.
///
/// Examples: `look`, `look at door`, `look earl`, `examine sign`, `look at key in inventory`
pub fn look(
    realm: Realm,
    player_ref: GameObjectRef,
    helpers: CommandHelpers,
) -> (Realm, Vec<PlayerOutput>) {
    let processor = helpers.command_line_processor;

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
        return inventory(
            realm,
            player_ref,
            CommandHelpers {
                command_line_processor: &mut CommandLineProcessor::new(player_ref, "inventory"),
                ..helpers
            },
        );
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
            } else if let Some(relative_direction) = RelativeDirection::from_string(target) {
                push_output_str!(
                    output,
                    player_ref,
                    match relative_direction {
                        RelativeDirection::Ahead => "You look ahead.\n",
                        RelativeDirection::Left => "You look to the left.\n",
                        RelativeDirection::Right => "You look to the right.\n",
                        RelativeDirection::Behind => "You look behind you.\n",
                        RelativeDirection::Up => "You look up.\n",
                        RelativeDirection::Down => "You look down.\n",
                    }
                );
                relative_direction.from(player.direction())
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

    (realm, output)
}
