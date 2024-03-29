use crate::{
    colors::Color,
    entity::{Character, Entity, EntityRef, EntityType, Item, Portal, Realm, Room, RoomFlags},
    player_output::PlayerOutput,
    utils::{
        angle_between_xy_vectors, colorize, compare_exit_names, describe_characters_relative_to,
        describe_entities_from_room, describe_items, description_for_position,
        first_item_is_plural, group_items_by_position, group_portals_by_position, join_sentence,
        visible_characters_through_portals_from_position, visible_items_from_position,
        visible_portals_from_position,
    },
    vector3d::Vector3D,
};
use std::{f64::consts::PI, iter::FromIterator};

/// Looks in a direction from the player's perspective.
pub fn look_in_direction(
    realm: &Realm,
    player_ref: EntityRef,
    direction: &Vector3D,
) -> Result<Vec<PlayerOutput>, String> {
    let (player, room) = realm.character_and_room_res(player_ref)?;

    /*TODO: binoculars are fun!
    let strength = room.event_multiplier(EventType::Visual);
    if (this.weapon && this.weapon.name === "binocular") {
        strength *= 4;
    }*/

    let mut lines = vec![];

    let items = visible_items_from_position(realm, room, direction);
    let portals = visible_portals_from_position(realm, room, direction);
    let descriptions = describe_entities_from_room(
        realm,
        &[&items[..], &portals[..]].concat(),
        room.entity_ref(),
    );
    if !descriptions.is_empty() {
        let sentence = join_sentence(&descriptions);
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
    Ok(vec![PlayerOutput::new_from_string(player_ref.id(), text)])
}

/// Looks at the given entity from the player's perspective.
pub fn look_at_entity(
    realm: &Realm,
    player_ref: EntityRef,
    entity_ref: EntityRef,
) -> Result<Vec<PlayerOutput>, String> {
    let entity = realm
        .entity(entity_ref)
        .ok_or("That object doesn't exist.")?;

    let mut text = match entity.entity_type() {
        EntityType::Item => look_at_item(realm, player_ref, entity.as_item().unwrap())?,
        EntityType::Portal => look_at_portal(realm, player_ref, entity.as_portal().unwrap())?,
        EntityType::Room => look_at_room(realm, player_ref, entity.as_room().unwrap())?,
        _ => {
            let description = entity.description();
            if description.is_empty() {
                format!("There is nothing special about the {}.", entity.name())
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
fn look_at_item(realm: &Realm, player_ref: EntityRef, item: &Item) -> Result<String, String> {
    let (_, room) = realm.character_and_room_res(player_ref)?;

    let mut lines = Vec::new();

    let description = item.description();
    if !description.is_empty() {
        lines.push(description.to_owned());
    }

    let looking_direction = item.position().to_vec();

    let show_nearby_entities = !item.position().is_default();
    if show_nearby_entities {
        room.items()
            .iter()
            .filter(|&&item_ref| item_ref != item.entity_ref())
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
                let name = portal.name_with_destination_from_room(room.entity_ref());
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
fn look_at_portal(realm: &Realm, player_ref: EntityRef, portal: &Portal) -> Result<String, String> {
    let (player, room) = realm.character_and_room_res(player_ref)?;

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
        .filter(|&&portal_ref| portal_ref != portal.entity_ref())
        .filter_map(|&portal_ref| realm.portal(portal_ref))
        .map(|portal| (portal, (portal.position(realm) - room.position())))
        .filter(|(_, vector)| looking_direction.angle(vector) < PI / 8.0)
        .for_each(|(portal, vector)| {
            let angle = angle_between_xy_vectors(&looking_direction, &vector);
            let side = if angle > 0.0 { "right" } else { "left" };
            let name = portal.name_with_destination_from_room(room.entity_ref());
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
            portal.name_from_room(room.entity_ref())
        ));
    }

    Ok(lines.join(" "))
}

/// Looks at the room from the player's perspective.
///
/// Assumes the player is currently in the room.
fn look_at_room(realm: &Realm, player_ref: EntityRef, room: &Room) -> Result<String, String> {
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

    let items_description = create_items_description(realm, &player.character, room);
    if !items_description.is_empty() {
        if !text.ends_with(' ') && !text.ends_with('\n') {
            text.push(' ');
        }
        text.push_str(&items_description);
    }

    if let Some(characters_description) =
        create_distant_characters_description(realm, &player.character, room)
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
    player: &Character,
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
        .filter(|portal| !portal.is_hidden_from_room(room.entity_ref()))
        .map(|portal| portal.name_from_room(room.entity_ref()))
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

fn create_items_description(realm: &Realm, player: &Character, room: &Room) -> String {
    let mut grouped_entities = group_items_by_position(realm, player, room);

    if room.has_flags(RoomFlags::DynamicPortalDescriptions) {
        for (position, portal_refs) in group_portals_by_position(realm, player, room) {
            if let Some(items) = grouped_entities.get_mut(&position) {
                for portal_ref in portal_refs {
                    items.push(portal_ref);
                }
            } else {
                grouped_entities.insert(position, portal_refs);
            }
        }
    }

    let mut sorted_entities = Vec::from_iter(grouped_entities);
    sorted_entities.sort();
    sorted_entities
        .iter()
        .map(|(position, entity_refs)| {
            let (prefix, singular_verb, plural_verb) = description_for_position(*position);
            let verb = if first_item_is_plural(realm, entity_refs) {
                plural_verb
            } else {
                singular_verb
            };

            let item_descriptions =
                describe_entities_from_room(realm, entity_refs, room.entity_ref());

            format!("{} {} {}.", prefix, verb, join_sentence(&item_descriptions))
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
    player_ref: EntityRef,
    room: &Room,
) -> Option<String> {
    let other_character_names: Vec<EntityRef> = room
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
            join_sentence(&describe_items(realm, &other_character_names))
        ))
    }
}
