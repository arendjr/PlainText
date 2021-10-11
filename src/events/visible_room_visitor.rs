use crate::{
    entity::{Entity, EntityType, Realm, Room, RoomFlags},
    player_output::PlayerOutput,
    vector3d::Vector3D,
};
use std::f64::consts::TAU;

use super::{event::Event, EventType};

pub fn visit_rooms(realm: &Realm, event: &dyn Event, strength: f32) -> Option<Vec<PlayerOutput>> {
    let mut rooms_to_visit = event
        .origins()
        .into_iter()
        .filter_map(|room| realm.room(room).map(|room| (room, strength)))
        .collect::<Vec<_>>();
    let mut room_visit_index = 0;
    loop {
        let (room, strength) = rooms_to_visit[room_visit_index];
        if let Some(next_rooms) = visit_room(realm, room, event, strength) {
            for (room, strength) in next_rooms {
                if let Some(entry) = rooms_to_visit.iter_mut().find(|(r, _)| r.id() == room.id()) {
                    if entry.1 < strength {
                        entry.1 = strength;
                    }
                } else {
                    rooms_to_visit.push((room, strength));
                }
            }
        }

        room_visit_index += 1;
        if room_visit_index >= rooms_to_visit.len() {
            break;
        }
    }

    let mut output = vec![];

    for (room, strength) in rooms_to_visit {
        if let Some(mut room_output) = notify_characters(realm, room, event, strength) {
            output.append(&mut room_output);
        }
    }

    Some(output)
}

fn visit_room<'a>(
    realm: &'a Realm,
    room: &Room,
    event: &dyn Event,
    strength: f32,
) -> Option<Vec<(&'a Room, f32)>> {
    let strength = strength * room.event_multiplier(EventType::Visual);
    if strength < 0.1 {
        return None; // Too weak to continue.
    }

    let mut next_rooms = Vec::new();

    for portal in room.portals() {
        let portal = match realm.portal(*portal) {
            Some(portal) if portal.can_see_through() => portal,
            _ => continue,
        };

        if let Some(opposite_room) = realm.room(portal.opposite_of(room.entity_ref())) {
            if is_within_sight(realm, event, room, opposite_room) {
                let propagated_strength = strength * portal.event_multiplier(EventType::Visual);
                if propagated_strength >= 0.1 {
                    next_rooms.push((opposite_room, propagated_strength));
                }
            }
        }
    }

    Some(next_rooms)
}

fn is_within_sight(
    realm: &Realm,
    event: &dyn Event,
    source_room: &Room,
    target_room: &Room,
) -> bool {
    let origins = event.origins();
    if origins.contains(&source_room.entity_ref()) {
        return true;
    }

    let target_vector = (target_room.position() - source_room.position()).normalized();

    for origin in origins {
        let origin = match realm.room(origin) {
            Some(room) => room,
            None => continue,
        };

        if is_visible_from(origin, source_room, &target_vector) {
            return true;
        }
    }

    false
}

fn is_visible_from(room: &Room, source_room: &Room, target_vector: &Vector3D) -> bool {
    let source_vector = (source_room.position() - room.position()).normalized();
    if &source_vector == target_vector {
        return true;
    }

    if source_room.has_flags(RoomFlags::HasWalls) {
        if target_vector.x != source_vector.x || target_vector.y != source_vector.y {
            return false;
        }
    } else if target_vector.z == source_vector.z {
        return true;
    }

    (!source_room.has_flags(RoomFlags::HasCeiling) && target_vector.z >= source_vector.z)
        || (!source_room.has_flags(RoomFlags::HasFloor) && target_vector.z <= source_vector.z)
}

fn notify_characters(
    realm: &Realm,
    room: &Room,
    event: &dyn Event,
    strength: f32,
) -> Option<Vec<PlayerOutput>> {
    let strength = strength * room.event_multiplier(EventType::Visual);
    if strength < 0.1 {
        return None;
    }

    let mut output = vec![];

    for character_ref in room.characters() {
        if event.excluded_characters().contains(character_ref) {
            continue;
        }

        // Make sure the character is actually looking in the direction of the event:
        let character = realm.character(*character_ref)?;
        if !event
            .origins()
            .into_iter()
            .filter_map(|origin| realm.room(origin))
            .any(|origin| {
                let event_direction = origin.position() - room.position();
                if event_direction.is_default() {
                    true // Assume everyone at the origin can see what is going on.
                } else {
                    let angle = character.direction().angle(&event_direction);
                    angle <= TAU / 8.0
                }
            })
        {
            continue;
        }

        let mut message = event.description_for_strength_and_character_in_room(
            realm,
            strength,
            *character_ref,
            room.entity_ref(),
        )?;
        if character_ref.entity_type() == EntityType::Player {
            message.push('\n');
            output.push(PlayerOutput::new_from_string(character_ref.id(), message));
        }
    }

    Some(output)
}
