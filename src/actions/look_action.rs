use crate::colors::Color;
use crate::direction_utils::compare_exit_names;
use crate::game_object::{Character, GameObject, GameObjectRef, GameObjectType};
use crate::objects::{Realm, Room, RoomFlags};
use crate::player_output::PlayerOutput;
use crate::text_utils::{colorize, describe_items_from_room, first_item_is_plural, join_sentence};
use crate::visual_utils::{
    describe_characters_relative_to, description_for_position, group_items_by_position,
    group_portals_by_position, visible_characters_from_position,
};

/**
 * Look at the given object from the player's perspective.
 */
pub fn look(
    realm: Realm,
    player_ref: GameObjectRef,
    object_ref: GameObjectRef,
    output: &mut Vec<PlayerOutput>,
) -> Realm {
    match realm.object(object_ref) {
        Some(object) => match object.object_type() {
            GameObjectType::Room => {
                look_at_room(realm, player_ref, object.as_room().unwrap(), output)
            }
            _ => {
                let description = object.description();
                output.push(PlayerOutput::new_from_string(
                    player_ref.id(),
                    if description.is_empty() {
                        format!("There is nothing special about the {}.", object.name())
                    } else {
                        description.to_owned()
                    },
                ));
                realm
            }
        },
        None => realm,
    }
}

fn look_at_room(
    realm: Realm,
    player_ref: GameObjectRef,
    room: &Room,
    output: &mut Vec<PlayerOutput>,
) -> Realm {
    let player = unwrap_or_return!(realm.character(player_ref), realm);

    let mut text = if room.name().is_empty() {
        room.description().to_owned()
    } else {
        format!(
            "\n{}\n\n{}",
            colorize(room.name(), Color::Teal),
            room.description()
        )
    };

    let items_description = create_items_description(&realm, player, room);
    if !items_description.is_empty() {
        if !text.ends_with(' ') && !text.ends_with('\n') {
            text.push(' ');
        }
        text.push_str(&items_description);
    }

    if let Some(characters_description) =
        create_distant_characters_description(&realm, player, room)
    {
        if !text.ends_with(' ') && !text.ends_with('\n') {
            text.push(' ');
        }
        text.push_str(&characters_description);
    }

    text.push('\n');

    if let Some(exits_description) = create_exits_description(&realm, room) {
        text.push_str(&exits_description);
    }

    if let Some(characters_description) =
        create_other_characters_description(&realm, player_ref, room)
    {
        text.push_str(&characters_description);
    }

    output.push(PlayerOutput::new_from_string(player_ref.id(), text));

    realm
}

fn create_distant_characters_description(
    realm: &Realm,
    player: &dyn Character,
    room: &Room,
) -> Option<String> {
    if room.has_flags(RoomFlags::DistantCharacterDescriptions) {
        let distant_characters = visible_characters_from_position(realm, player, room);
        if distant_characters.is_empty() {
            None
        } else {
            Some(describe_characters_relative_to(
                realm,
                distant_characters,
                player,
            ))
        }
    } else {
        None
    }
}

fn create_exits_description(realm: &Realm, room: &Room) -> Option<String> {
    let mut exit_names: Vec<&str> = room
        .portals()
        .iter()
        .flat_map(|portal_ref| realm.portal(*portal_ref))
        .filter(|portal| !portal.is_hidden_from_room(room.object_ref()))
        .map(|portal| portal.name_from_room(room.object_ref()))
        .collect();
    if exit_names.is_empty() {
        None
    } else {
        exit_names.sort_by(compare_exit_names);
        Some(colorize(
            &format!("Obvious exits: {}.\n", exit_names.join(", ")),
            Color::Green,
        ))
    }
}

fn create_items_description(realm: &Realm, player: &dyn Character, room: &Room) -> String {
    let mut grouped_items = group_items_by_position(realm, player, room);
    if room.has_flags(RoomFlags::DynamicPortalDescriptions) {
        for (position, portal_refs) in group_portals_by_position(realm, player, room) {
            if let Some(items) = grouped_items.get_mut(&position) {
                for portal_ref in portal_refs {
                    items.push(portal_ref);
                }
            } else {
                grouped_items.insert(position, portal_refs);
            }
        }
    }

    grouped_items
        .iter()
        .map(|(position, item_refs)| {
            let (prefix, singular_verb, plural_verb) = description_for_position(*position);
            let verb = if first_item_is_plural(realm, item_refs) {
                plural_verb
            } else {
                singular_verb
            };

            let item_descriptions = describe_items_from_room(realm, item_refs, room.object_ref());

            format!(
                "{} {} {}.",
                prefix,
                verb,
                join_sentence(
                    item_descriptions
                        .iter()
                        .map(|string| string.as_str())
                        .collect()
                )
            )
            .replace("there is", "there's")
        })
        .fold("".to_owned(), |result, string| {
            if result.is_empty() {
                string
            } else if result.ends_with(' ') || result.ends_with('\n') {
                result + &string
            } else {
                format!("{} {}", result, string)
            }
        })
}

fn create_other_characters_description(
    realm: &Realm,
    player_ref: GameObjectRef,
    room: &Room,
) -> Option<String> {
    let other_character_names: Vec<&str> = room
        .characters()
        .iter()
        .filter(|character_ref| **character_ref != player_ref)
        .filter_map(|character_ref| realm.character(*character_ref))
        .map(|character| character.name())
        .collect();
    if other_character_names.is_empty() {
        None
    } else {
        Some(format!(
            "You see {}.\n",
            join_sentence(other_character_names)
        ))
    }
}
