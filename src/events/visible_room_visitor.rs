use std::f64::consts::TAU;

use crate::{
    game_object::GameObject,
    objects::{Realm, Room},
    player_output::PlayerOutput,
};

use super::{event::VisualEvent, EventType};

pub fn visit_rooms(
    realm: &Realm,
    event: &dyn VisualEvent,
    strength: f32,
) -> Option<Vec<PlayerOutput>> {
    let mut rooms_to_visit = vec![(realm.room(event.origin())?, strength)];
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
    event: &dyn VisualEvent,
    strength: f32,
) -> Option<Vec<(&'a Room, f32)>> {
    let strength = strength * room.event_multiplier(event.event_type());
    if strength < 0.1 {
        return None; // Too weak to continue.
    }

    let mut next_rooms = Vec::new();

    for portal in room.portals() {
        let portal = match realm.portal(*portal) {
            Some(portal) if portal.can_see_through() => portal,
            _ => continue,
        };

        if let (Some(room1), Some(room2)) = (realm.room(portal.room()), realm.room(portal.room2()))
        {
            let opposite_room = if room.id() == room1.id() {
                room2
            } else {
                room1
            };
            if event.is_within_sight(realm, opposite_room, room) {
                let propagated_strength = strength * portal.event_multiplier(EventType::Visual);
                if propagated_strength >= 0.1 {
                    next_rooms.push((opposite_room, propagated_strength));
                }
            }
        }
    }

    Some(next_rooms)
}

fn notify_characters(
    realm: &Realm,
    room: &Room,
    event: &dyn VisualEvent,
    strength: f32,
) -> Option<Vec<PlayerOutput>> {
    let strength = strength * room.event_multiplier(event.event_type());
    if strength < 0.1 {
        return None;
    }

    let mut output = vec![];
    let origin = realm.room(event.origin())?;

    for character in room.characters() {
        if event.excluded_characters().contains(character) {
            continue;
        }

        // Make sure the character is actually looking in the direction of the event:
        let character = realm.character(*character)?;
        let event_direction = origin.position() - room.position();
        let angle = character.direction().angle(&event_direction);
        if angle > TAU / 8.0 {
            continue;
        }

        let mut message = event.description_for_strength_and_character_in_room(
            realm,
            strength,
            character.object_ref(),
            room.object_ref(),
        )?;
        if let Some(player) = character.as_player() {
            message.push('\n');
            output.push(PlayerOutput::new_from_string(player.id(), message));
        }
    }

    Some(output)
}
