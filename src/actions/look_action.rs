use std::{f64::consts::PI, iter::FromIterator};

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
) -> Result<Vec<PlayerOutput>, String> {
    let (player, room) = realm.player_and_room_res(player_ref)?;

    /*TODO: binoculars are fun!
    let strength = room.event_multiplier(EventType::Visual);
    if (this.weapon && this.weapon.name === "binocular") {
        strength *= 4;
    }*/

    let mut lines = vec![];

    let items = visible_items_from_position(realm, room, direction);
    let portals = visible_portals_from_position(realm, room, direction);
    let descriptions = describe_objects_from_room(
        realm,
        &[&items[..], &portals[..]].concat(),
        room.object_ref(),
    );
    if !descriptions.is_empty() {
        let sentence = join_sentence(descriptions);
        lines.push(format!("You see {}.", sentence));
    }

    let characters = visible_characters_through_portals_from_position(realm, room, direction);
    if !characters.is_empty() {
        lines.push(describe_characters_relative_to(realm, characters, player));
    }

    if lines.is_empty() {
        lines.push("There's nothing to see in that direction.".to_owned());
    }

    let mut text = lines.join("\n");
    text.push('\n');
    Ok(vec![PlayerOutput::new_from_string(player.id(), text)])
}

/// Looks at the given object from the player's perspective.
pub fn look_at_object(
    realm: &Realm,
    player_ref: GameObjectRef,
    object_ref: GameObjectRef,
) -> Result<Vec<PlayerOutput>, String> {
    let object = realm
        .object(object_ref)
        .ok_or("That object doesn't exist.")?;

    let mut text = match object.object_type() {
        GameObjectType::Item => look_at_item(realm, player_ref, object.as_item().unwrap())?,
        GameObjectType::Portal => look_at_portal(realm, player_ref, object.as_portal().unwrap())?,
        GameObjectType::Room => look_at_room(realm, player_ref, object.as_room().unwrap())?,
        _ => {
            let description = object.description();
            if description.is_empty() {
                format!("There is nothing special about the {}.", object.name())
            } else {
                description.to_owned()
            }
        }
    };

    if !text.ends_with('\n') {
        text.push('\n');
    }

    Ok(vec![PlayerOutput::new_from_string(player_ref.id(), text)])
}

/// Looks at the item from the player's perspective.
///
/// Assumes the player is currently in the same room.
fn look_at_item(realm: &Realm, player_ref: GameObjectRef, item: &Item) -> Result<String, String> {
    let (_, room) = realm.player_and_room_res(player_ref)?;

    let mut lines = Vec::new();

    let description = item.description();
    if !description.is_empty() {
        lines.push(description.to_owned());
    }

    let looking_direction = item.position().to_vec();

    let show_nearby_objects = !item.position().is_default();
    if show_nearby_objects {
        room.items()
            .iter()
            .filter(|&&item_ref| item_ref != item.object_ref())
            .filter_map(|&item_ref| realm.item(item_ref))
            .filter(|item| looking_direction.angle(&item.position().to_vec()) < PI / 8.0)
            .for_each(|item| {
                let angle = angle_between_xy_vectors(&looking_direction, &item.position().to_vec());
                lines.push(format!(
                    "On its {} there's {}.",
                    if angle > 0.0 { "right" } else { "left" },
                    item.indefinite_name()
                ));
            });
        room.portals()
            .iter()
            .filter_map(|&portal_ref| realm.portal(portal_ref))
            .map(|portal| (portal, (portal.position(realm) - room.position())))
            .filter(|(_, vector)| looking_direction.angle(vector) < PI / 8.0)
            .for_each(|(portal, vector)| {
                let angle = angle_between_xy_vectors(&looking_direction, &vector);
                let side = if angle > 0.0 { "right" } else { "left" };
                let name = portal.name_with_destination_from_room(room.object_ref());
                lines.push(format!("On its {} is {}.", side, name));
            });
    }

    if lines.is_empty() {
        lines.push(format!(
            "There is nothing special about the {}.",
            item.name()
        ));
    }

    Ok(lines.join(" "))
}

/// Looks at the portal from the player's perspective.
///
/// Assumes the player is currently in one of the adjacent rooms.
fn look_at_portal(
    realm: &Realm,
    player_ref: GameObjectRef,
    portal: &Portal,
) -> Result<String, String> {
    let (player, room) = realm.player_and_room_res(player_ref)?;

    let mut lines = Vec::new();

    let description = portal.description();
    if !description.is_empty() {
        lines.push(description.to_owned());
    }

    let looking_direction = portal.position(realm) - room.position();

    room.items()
        .iter()
        .filter_map(|&item_ref| realm.item(item_ref))
        .filter(|item| looking_direction.angle(&item.position().to_vec()) < PI / 8.0)
        .for_each(|item| {
            let angle = angle_between_xy_vectors(&looking_direction, &item.position().to_vec());
            lines.push(format!(
                "On its {} there's {}.",
                if angle > 0.0 { "right" } else { "left" },
                item.indefinite_name()
            ));
        });
    room.portals()
        .iter()
        .filter(|&&portal_ref| portal_ref != portal.object_ref())
        .filter_map(|&portal_ref| realm.portal(portal_ref))
        .map(|portal| (portal, (portal.position(realm) - room.position())))
        .filter(|(_, vector)| looking_direction.angle(vector) < PI / 8.0)
        .for_each(|(portal, vector)| {
            let angle = angle_between_xy_vectors(&looking_direction, &vector);
            let side = if angle > 0.0 { "right" } else { "left" };
            let name = portal.name_with_destination_from_room(room.object_ref());
            lines.push(format!("On its {} is {}.", side, name));
        });

    if portal.can_see_through() {
        let characters =
            visible_characters_through_portals_from_position(realm, room, &looking_direction);
        let characters_description = describe_characters_relative_to(realm, characters, player);
        if !characters_description.is_empty() {
            lines.push(characters_description);
        }
    }

    if lines.is_empty() {
        lines.push(format!(
            "There is nothing special about the {}.",
            portal.name_from_room(room.object_ref())
        ));
    }

    Ok(lines.join(" "))
}

/// Looks at the room from the player's perspective.
///
/// Assumes the player is currently in the room.
fn look_at_room(realm: &Realm, player_ref: GameObjectRef, room: &Room) -> Result<String, String> {
    let player = realm.player_res(player_ref)?;

    let mut text = if room.name().is_empty() {
        room.description().to_owned()
    } else {
        format!(
            "\n{}\n\n{}",
            colorize(room.name(), Color::Teal),
            room.description()
        )
    };

    let items_description = create_items_description(realm, player, room);
    if !items_description.is_empty() {
        if !text.ends_with(' ') && !text.ends_with('\n') {
            text.push(' ');
        }
        text.push_str(&items_description);
    }

    if let Some(characters_description) = create_distant_characters_description(realm, player, room)
    {
        if !text.ends_with(' ') && !text.ends_with('\n') {
            text.push(' ');
        }
        text.push_str(&characters_description);
    }

    text.push('\n');

    if let Some(exits_description) = create_exits_description(realm, room) {
        text.push_str(&exits_description);
    }

    if let Some(characters_description) =
        create_other_characters_description(realm, player_ref, room)
    {
        text.push_str(&characters_description);
    }

    Ok(text)
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

    let mut sorted_objects = Vec::from_iter(grouped_objects);
    sorted_objects.sort();
    sorted_objects
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
                .replace(". You ", " and you ")
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
