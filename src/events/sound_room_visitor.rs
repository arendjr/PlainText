use crate::{
    entity::{Entity, EntityType, Realm, Room},
    player_output::PlayerOutput,
};

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
        if let Some(next_rooms) = visit_room(realm, room, strength) {
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

fn visit_room<'a>(realm: &'a Realm, room: &Room, strength: f32) -> Option<Vec<(&'a Room, f32)>> {
    let strength = strength * room.event_multiplier(EventType::Sound);
    if strength < 0.1 {
        return None; // Too weak to continue.
    }

    let mut next_rooms = Vec::new();

    for portal in room.portals() {
        let portal = match realm.portal(*portal) {
            Some(portal) if portal.can_hear_through() => portal,
            _ => continue,
        };

        if let Some(opposite_room) = realm.room(portal.opposite_of(room.entity_ref())) {
            let distance = (opposite_room.position() - room.position()).len();
            let distance_multiplier = 1.0 / (distance as f32 / 3.0);
            let propagated_strength =
                strength * distance_multiplier * portal.event_multiplier(EventType::Sound);
            if propagated_strength >= 0.1 {
                next_rooms.push((opposite_room, propagated_strength));
            }
        }
    }

    Some(next_rooms)
}

fn notify_characters(
    realm: &Realm,
    room: &Room,
    event: &dyn Event,
    strength: f32,
) -> Option<Vec<PlayerOutput>> {
    let strength = strength * room.event_multiplier(EventType::Sound);
    if strength < 0.1 {
        return None;
    }

    let mut output = vec![];

    for character in room.characters() {
        if event.excluded_characters().contains(character)
            || character.entity_type() != EntityType::Player
        {
            continue;
        }

        let mut message = event.description_for_strength_and_character_in_room(
            realm,
            strength,
            *character,
            room.entity_ref(),
        )?;
        message.push('\n');
        output.push(PlayerOutput::new_from_string(character.id(), message));
    }

    Some(output)
}
