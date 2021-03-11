use std::f64::consts::PI;

use crate::colors::Color;
use crate::direction_utils::compare_exit_names;
use crate::game_object::{Character, GameObject, GameObjectRef, GameObjectType};
use crate::objects::{Item, Portal, Realm, Room, RoomFlags};
use crate::player_output::PlayerOutput;
use crate::text_utils::{
    colorize, describe_items, describe_objects_from_room, first_item_is_plural, join_sentence,
};
use crate::vector3d::Vector3D;
use crate::vector_utils::angle_between_xy_vectors;
use crate::vision_utils::{
    describe_characters_relative_to, description_for_position, group_items_by_position,
    group_portals_by_position, visible_characters_through_portals_from_position,
    visible_items_from_position, visible_portals_from_position,
};

/// Looks in a direction from the player's perspective.
pub fn look_in_direction(
    realm: &Realm,
    player_ref: GameObjectRef,
    direction: &Vector3D,
    output: &mut Vec<PlayerOutput>,
) {
    let player = unwrap_or_return!(realm.character(player_ref), ());
    let room = unwrap_or_return!(realm.room(player.current_room()), ());

    /*TODO: binoculars are fun!
    let strength = room.event_multiplier(EventType::Visual);
    if (this.weapon && this.weapon.name === "binocular") {
        strength *= 4;
    }*/

    let items = visible_items_from_position(realm, room, &direction);
    let portals = visible_portals_from_position(realm, room, &direction);
    let descriptions = describe_objects_from_room(
        realm,
        &[&items[..], &portals[..]].concat(),
        room.object_ref(),
    );
    if !descriptions.is_empty() {
        let sentence = join_sentence(descriptions);
        push_output_string!(output, player_ref, format!("You see {}.\n", sentence));
    }

    let characters = visible_characters_through_portals_from_position(realm, &room, &direction);
    if !characters.is_empty() {
        push_output_string!(
            output,
            player_ref,
            describe_characters_relative_to(realm, characters, player)
        );
    }
}

/// Looks at the given object from the player's perspective.
pub fn look_at_object(
    realm: &Realm,
    player_ref: GameObjectRef,
    object_ref: GameObjectRef,
    output: &mut Vec<PlayerOutput>,
) {
    let object = unwrap_or_return!(realm.object(object_ref), ());
    match object.object_type() {
        GameObjectType::Item => look_at_item(realm, player_ref, object.as_item().unwrap(), output),
        GameObjectType::Portal => {
            look_at_portal(realm, player_ref, object.as_portal().unwrap(), output)
        }
        GameObjectType::Room => look_at_room(realm, player_ref, object.as_room().unwrap(), output),
        _ => {
            let description = object.description();
            push_output_string!(
                output,
                player_ref,
                if description.is_empty() {
                    format!("There is nothing special about the {}.\n", object.name())
                } else {
                    format!("{}\n", description)
                }
            );
        }
    }
}

/// Looks at the item from the player's perspective.
///
/// Assumes the player is currently in the same room.
fn look_at_item(
    realm: &Realm,
    player_ref: GameObjectRef,
    item: &Item,
    output: &mut Vec<PlayerOutput>,
) {
    let player = unwrap_or_return!(realm.player(player_ref), ());
    let current_room = unwrap_or_return!(realm.room(player.current_room()), ());

    let description = item.description();
    push_output_string!(
        output,
        player_ref,
        if description.is_empty() {
            format!("There is nothing special about the {}.\n", item.name())
        } else {
            format!("{}\n", description)
        }
    );

    let looking_direction = item.position().to_vec();

    let show_nearby_objects = !item.position().is_default();
    if show_nearby_objects {
        current_room
            .items()
            .iter()
            .filter(|&&item_ref| item_ref != item.object_ref())
            .filter_map(|&item_ref| realm.item(item_ref))
            .filter(|item| looking_direction.angle(&item.position().to_vec()) < PI / 8.0)
            .for_each(|item| {
                let angle = angle_between_xy_vectors(&looking_direction, &item.position().to_vec());
                let side = if angle > 0.0 { "right" } else { "left" };
                push_output_string!(
                    output,
                    player_ref,
                    format!("On its {} there's {}.", side, item.indefinite_name())
                );
            });
        current_room
            .portals()
            .iter()
            .filter_map(|&portal_ref| realm.portal(portal_ref))
            .map(|portal| (portal, (portal.position(&realm) - current_room.position())))
            .filter(|(_, vector)| looking_direction.angle(&vector) < PI / 8.0)
            .for_each(|(portal, vector)| {
                let angle = angle_between_xy_vectors(&looking_direction, &vector);
                let side = if angle > 0.0 { "right" } else { "left" };
                let name = portal.name_with_destination_from_room(current_room.object_ref());
                push_output_string!(output, player_ref, format!("On its {} is {}.", side, name));
            });
    }
}

/// Looks at the portal from the player's perspective.
///
/// Assumes the player is currently in one of the adjacent rooms.
fn look_at_portal(
    realm: &Realm,
    player_ref: GameObjectRef,
    portal: &Portal,
    output: &mut Vec<PlayerOutput>,
) {
    let player = unwrap_or_return!(realm.player(player_ref), ());
    let current_room = unwrap_or_return!(realm.room(player.current_room()), ());

    let description = portal.description();
    push_output_string!(
        output,
        player_ref,
        if description.is_empty() {
            format!("There is nothing special about the {}.\n", portal.name())
        } else {
            format!("{}\n", description)
        }
    );

    let looking_direction = portal.position(&realm) - current_room.position();

    current_room
        .items()
        .iter()
        .filter_map(|&item_ref| realm.item(item_ref))
        .filter(|item| looking_direction.angle(&item.position().to_vec()) < PI / 8.0)
        .for_each(|item| {
            let angle = angle_between_xy_vectors(&looking_direction, &item.position().to_vec());
            let side = if angle > 0.0 { "right" } else { "left" };
            push_output_string!(
                output,
                player_ref,
                format!("On its {} there's {}.", side, item.indefinite_name())
            );
        });
    current_room
        .portals()
        .iter()
        .filter(|&&portal_ref| portal_ref != portal.object_ref())
        .filter_map(|&portal_ref| realm.portal(portal_ref))
        .map(|portal| (portal, (portal.position(&realm) - current_room.position())))
        .filter(|(_, vector)| looking_direction.angle(&vector) < PI / 8.0)
        .for_each(|(portal, vector)| {
            let angle = angle_between_xy_vectors(&looking_direction, &vector);
            let side = if angle > 0.0 { "right" } else { "left" };
            let name = portal.name_with_destination_from_room(current_room.object_ref());
            push_output_string!(output, player_ref, format!("On its {} is {}.", side, name));
        });

    if portal.can_see_through() {
        let characters = visible_characters_through_portals_from_position(
            &realm,
            current_room,
            &looking_direction,
        );
        let characters_description = describe_characters_relative_to(&realm, characters, player);
        if !characters_description.is_empty() {
            push_output_string!(output, player_ref, characters_description);
        }
    }
}

/// Looks at the room from the player's perspective.
///
/// Assumes the player is currently in the room.
fn look_at_room(
    realm: &Realm,
    player_ref: GameObjectRef,
    room: &Room,
    output: &mut Vec<PlayerOutput>,
) {
    let player = unwrap_or_return!(realm.character(player_ref), ());

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

    push_output_string!(output, player_ref, text);
}

fn create_distant_characters_description(
    realm: &Realm,
    player: &dyn Character,
    room: &Room,
) -> Option<String> {
    if room.has_flags(RoomFlags::DistantCharacterDescriptions) {
        let distant_characters =
            visible_characters_through_portals_from_position(realm, room, player.direction());
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
    let mut grouped_objects = group_items_by_position(realm, player, room);
    if room.has_flags(RoomFlags::DynamicPortalDescriptions) {
        for (position, portal_refs) in group_portals_by_position(realm, player, room) {
            if let Some(items) = grouped_objects.get_mut(&position) {
                for portal_ref in portal_refs {
                    items.push(portal_ref);
                }
            } else {
                grouped_objects.insert(position, portal_refs);
            }
        }
    }

    grouped_objects
        .iter()
        .map(|(position, object_refs)| {
            let (prefix, singular_verb, plural_verb) = description_for_position(*position);
            let verb = if first_item_is_plural(realm, object_refs) {
                plural_verb
            } else {
                singular_verb
            };

            let item_descriptions =
                describe_objects_from_room(realm, object_refs, room.object_ref());

            format!("{} {} {}.", prefix, verb, join_sentence(item_descriptions))
                .replace("there is", "there's")
        })
        .fold(String::new(), |result, string| {
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
    let other_character_names: Vec<GameObjectRef> = room
        .characters()
        .iter()
        .filter(|character_ref| **character_ref != player_ref)
        .cloned()
        .collect();
    if other_character_names.is_empty() {
        None
    } else {
        Some(format!(
            "You see {}.\n",
            join_sentence(describe_items(realm, &other_character_names))
        ))
    }
}
