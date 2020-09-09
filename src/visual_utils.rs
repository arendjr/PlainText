use std::collections::{HashMap, HashSet};
use std::f64::consts::PI;
use std::hash::{Hash, Hasher};
use std::iter::FromIterator;

use crate::direction_utils::is_direction;
use crate::events::EventType;
use crate::game_object::{Character, GameObject, GameObjectRef};
use crate::objects::{ItemFlags, Player, Portal, Realm, Room, RoomFlags};
use crate::vector3d::Vector3D;
use crate::vector_utils::angle_between_xy_vectors;

const UNDER_QUARTER_PI: f64 = PI / 4.01;
const OVER_QUARTER_PI: f64 = PI / 3.99;

struct CharacterWithStrengthAndDistance {
    character: GameObjectRef,
    strength: f32,
    distance: i32,
}

impl Eq for CharacterWithStrengthAndDistance {}

impl Hash for CharacterWithStrengthAndDistance {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.character.hash(state)
    }
}

impl PartialEq for CharacterWithStrengthAndDistance {
    fn eq(&self, other: &Self) -> bool {
        self.character == other.character
    }
}

#[derive(Clone, Copy, Debug, Eq, Hash, PartialEq)]
pub enum Position {
    Left,
    Right,
    Ahead,
    Behind,
    Center,
    Above,
    LeftWall,
    RightWall,
    Wall,
    Ceiling,
    Distance,
    Roof,
}

fn characters_visible_through_portal(
    realm: &Realm,
    character: &Player,
    source_room: &Room,
    portal: &Portal,
    strength: f32,
    excluded_rooms: &HashSet<GameObjectRef>,
) -> HashSet<CharacterWithStrengthAndDistance> {
    let room = unwrap_or_return!(
        realm.room(portal.opposite_of(source_room.object_ref())),
        HashSet::new()
    );
    let room_strength = if strength != 0.0 {
        strength
    } else {
        source_room.event_multiplier(EventType::Visual)
    } * portal.event_multiplier(EventType::Visual)
        * room.event_multiplier(EventType::Visual);
    if room_strength < 0.1 {
        return HashSet::new();
    }

    let vector1: Vector3D = room.position() - source_room.position();
    let distance = vector1.len();

    let mut visited_rooms = HashSet::new();
    visited_rooms.insert(source_room.object_ref());
    visited_rooms.insert(room.object_ref());
    for room_ref in excluded_rooms {
        visited_rooms.insert(*room_ref);
    }

    let mut characters = HashSet::from_iter(
        room.characters()
            .iter()
            .filter_map(|character_ref| realm.character(*character_ref))
            .map(|character| CharacterWithStrengthAndDistance {
                character: character.object_ref(),
                strength: room_strength,
                distance,
            }),
    );

    for next_portal in room
        .portals()
        .iter()
        .filter_map(|portal_ref| realm.portal(*portal_ref))
    {
        if !next_portal.can_see_through() {
            continue;
        }

        let next_room = unwrap_or_continue!(realm.room(next_portal.opposite_of(room.object_ref())));
        if next_room.id() == source_room.id() || visited_rooms.contains(&next_room.object_ref()) {
            continue;
        }

        let vector2 = next_room.position() - room.position();
        if room.has_flags(RoomFlags::HasWalls) {
            // If there are walls, we can only follow line-of-sight in a straight line:
            if vector1.x != vector2.x || vector1.y != vector2.y {
                continue;
            }
        } else {
            // In an open area, we only verify the next room is within field of view:
            let character_room = unwrap_or_continue!(realm.room(character.current_room()));
            let vector3 = next_room.position() - character_room.position();
            let angle = angle_between_xy_vectors(character.direction(), &vector3);
            if angle.abs() > UNDER_QUARTER_PI {
                continue;
            }
        }

        if (room.has_flags(RoomFlags::HasCeiling) && vector2.z > vector1.z)
            || (room.has_flags(RoomFlags::HasFloor) && vector2.z < vector1.z)
        {
            continue;
        }

        for character_with_strength_and_distance in characters_visible_through_portal(
            realm,
            character,
            &next_room,
            &next_portal,
            strength,
            &visited_rooms,
        ) {
            characters.insert(character_with_strength_and_distance);
        }
    }

    characters
}

pub fn description_for_position(position: Position) -> (&'static str, &'static str, &'static str) {
    match position {
        Position::Left => ("To your left", "is", "are"),
        Position::Right => ("To your right", "is", "are"),
        Position::Ahead => ("Ahead of you, there", "is", "are"),
        Position::Behind => ("Behind you", "is", "are"),
        Position::Above => ("Above you", "is", "are"),
        Position::LeftWall => ("On the left wall", "hangs", "hang"),
        Position::RightWall => ("On the right wall", "hangs", "hang"),
        Position::Wall => ("On the wall", "hangs", "hang"),
        Position::Ceiling => ("From the ceiling", "hangs", "hang"),
        Position::Distance => ("In the distance", "you see", "you see"),
        Position::Roof => ("On the roof", "you see", "you see"),
        _ => ("There", "is", "are"),
    }
}

pub fn group_items_by_position(
    realm: &Realm,
    player_ref: GameObjectRef,
    room_ref: GameObjectRef,
) -> HashMap<Position, Vec<GameObjectRef>> {
    let mut grouped_items: HashMap<Position, Vec<GameObjectRef>> = HashMap::new();
    let player = unwrap_or_return!(realm.player(player_ref), grouped_items);
    let room = unwrap_or_return!(realm.room(room_ref), grouped_items);

    for item_ref in room.items() {
        let item = unwrap_or_continue!(realm.item(*item_ref));
        if item.hidden() {
            continue;
        }

        let position = if item.has_flags(ItemFlags::AttachedToCeiling) {
            Position::Ceiling
        } else if item.has_flags(ItemFlags::AttachedToWall) {
            let angle = angle_between_xy_vectors(player.direction(), &item.position().to_vec());
            if item.position().x == 0 && item.position().y == 0
                || angle.abs() > 3.0 * OVER_QUARTER_PI
                || angle.abs() < UNDER_QUARTER_PI
            {
                Position::Wall
            } else if angle > 0.0 {
                Position::RightWall
            } else {
                Position::LeftWall
            }
        } else if item.position().x == 0 && item.position().y == 0 {
            Position::Center
        } else {
            let angle = angle_between_xy_vectors(player.direction(), &item.position().to_vec());
            if angle.abs() > 3.0 * OVER_QUARTER_PI {
                Position::Behind
            } else if angle.abs() < UNDER_QUARTER_PI {
                Position::Ahead
            } else if angle > 0.0 {
                Position::Right
            } else {
                Position::Left
            }
        };

        if let Some(items) = grouped_items.get_mut(&position) {
            items.push(item.object_ref());
        } else {
            grouped_items.insert(position, vec![item.object_ref()]);
        }
    }

    grouped_items
}

pub fn group_portals_by_position(
    realm: &Realm,
    player_ref: GameObjectRef,
    room_ref: GameObjectRef,
) -> HashMap<Position, Vec<GameObjectRef>> {
    let mut grouped_items: HashMap<Position, Vec<GameObjectRef>> = HashMap::new();
    let player = unwrap_or_return!(realm.player(player_ref), grouped_items);
    let room = unwrap_or_return!(realm.room(room_ref), grouped_items);

    for portal_ref in room.portals() {
        let portal = unwrap_or_continue!(realm.portal(*portal_ref));
        if portal.is_hidden_from_room(room.object_ref()) {
            continue;
        }

        let name = portal.name_from_room(room.object_ref());
        if is_direction(name) || name == "out" {
            continue;
        }

        let vector = portal.position() - room.position();
        let angle = angle_between_xy_vectors(player.direction(), &vector);
        let position = if angle.abs() > 3.0 * OVER_QUARTER_PI {
            Position::Behind
        } else if angle.abs() < UNDER_QUARTER_PI {
            Position::Ahead
        } else if angle > 0.0 {
            Position::Right
        } else {
            Position::Left
        };

        if let Some(items) = grouped_items.get_mut(&position) {
            items.push(portal.object_ref());
        } else {
            grouped_items.insert(position, vec![portal.object_ref()]);
        }
    }

    grouped_items
}

pub fn visible_characters_from_position(
    realm: &Realm,
    player_ref: GameObjectRef,
    room_ref: GameObjectRef,
) -> Vec<GameObjectRef> {
    let player = unwrap_or_return!(realm.player(player_ref), vec![]);
    let room = unwrap_or_return!(realm.room(room_ref), vec![]);

    let mut characters = HashSet::new();
    for portal_ref in room.portals() {
        let portal = unwrap_or_continue!(realm.portal(*portal_ref));
        if !portal.can_see_through() || portal.is_hidden_from_room(room.object_ref()) {
            continue;
        }

        let vector = portal.position() - room.position();
        let angle = angle_between_xy_vectors(player.direction(), &vector);
        if angle.abs() < UNDER_QUARTER_PI {
            for character_with_strength_and_distance in characters_visible_through_portal(
                realm,
                &player,
                &room,
                &portal,
                room.event_multiplier(EventType::Visual),
                &HashSet::new(),
            ) {
                characters.insert(character_with_strength_and_distance.character);
            }
        }
    }
    characters.into_iter().collect()
}
