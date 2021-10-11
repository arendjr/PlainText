use super::change_direction;
use crate::{
    actions,
    entity::{EntityRef, EntityType, Realm},
    events::{AudibleMovementEvent, VisualEvent, VisualMovementEvent},
    player_output::PlayerOutput,
    text_utils::{capitalize, definite_character_name},
    vector3d::Vector3D,
};
use std::collections::BTreeSet;

/// Makes the character enter the given portal.
pub fn enter_portal(
    realm: &mut Realm,
    character_ref: EntityRef,
    portal_ref: EntityRef,
    from_room_ref: EntityRef,
) -> Result<Vec<PlayerOutput>, String> {
    let portal = realm
        .portal(portal_ref)
        .ok_or("That portal doesn't exist.")?;

    if portal.can_open() && !portal.is_open() {
        return Err(format!(
            "The {} is closed.",
            portal.name_from_room(from_room_ref)
        ));
    } else if !portal.can_pass_through() {
        return Err("You cannot go there.".into());
    }

    let target_room_ref = portal.opposite_of(from_room_ref);
    enter_room(realm, character_ref, target_room_ref)
}

/// Makes the character enter the given room.
pub fn enter_room(
    realm: &mut Realm,
    character_ref: EntityRef,
    room_ref: EntityRef,
) -> Result<Vec<PlayerOutput>, String> {
    let character = realm
        .character(character_ref)
        .ok_or("The character doesn't exist.")?;

    let character_name = definite_character_name(realm, character_ref)?;

    let current_room_ref = character.current_room();
    if current_room_ref == room_ref {
        if let Some(room) = realm.room_mut(room_ref) {
            room.add_characters(&[character_ref]);
        }

        let mut visual_event = VisualEvent::new(room_ref);
        visual_event.set_description(
            &format!("{} arrived.", capitalize(&character_name)),
            &format!("You see {} arrive.", character_name),
            "You see someone arrive.",
        );
        visual_event.excluded_characters.push(character_ref);
        let visual_output = visual_event
            .fire(realm, 1.0)
            .ok_or_else(|| "You arrived, but nobody saw you.".to_owned())?;

        return Ok(visual_output);
    }

    let party = character
        .group()
        .and_then(|group| realm.group(group))
        .and_then(|group| match group.leader() == character_ref {
            true => Some(group.followers()),
            false => None,
        })
        .map(|followers| {
            let mut party = vec![character_ref];
            for follower in followers {
                let is_in_same_room = realm
                    .character(*follower)
                    .map(|follower| follower.current_room() == current_room_ref)
                    .unwrap_or(false);
                if is_in_same_room {
                    party.push(*follower);
                }
            }
            party
        })
        .unwrap_or_else(|| vec![character_ref]);
    let visual_subject = if party.len() > 1 {
        character.group().unwrap()
    } else {
        character_ref
    };

    for character in party.iter() {
        if let Some(character) = realm.character_mut(*character) {
            character.set_current_room(room_ref);
        }
    }

    let direction = match (realm.room(current_room_ref), realm.room(room_ref)) {
        (Some(current_room), Some(new_room)) => {
            let direction = (new_room.position() - current_room.position()).normalized();
            for character in party.iter() {
                change_direction(realm, *character, direction.clone());
            }
            direction
        }
        (_, _) => Vector3D::default(),
    };

    if let Some(current_room) = realm.room_mut(current_room_ref) {
        current_room.remove_characters(&party);
    }

    if let Some(room) = realm.room_mut(room_ref) {
        room.add_characters(&party);
    }

    let mut visual_event = VisualMovementEvent::new(visual_subject, current_room_ref, room_ref);
    visual_event.direction = direction.clone();
    if party.len() == 1 {
        visual_event.set_verb("walks", "is walking");
    } else {
        visual_event.set_verb("walk", "are walking");
    }
    visual_event.excluded_characters = party.clone();
    let visual_output = visual_event
        .fire(realm, 1.0)
        .ok_or_else(|| "You walked, but nobody saw you.".to_owned())?;

    let visual_witnesses = visual_output
        .iter()
        .map(|output| EntityRef(EntityType::Player, output.player_id))
        .collect::<BTreeSet<_>>();

    let mut sound_event = AudibleMovementEvent::new(current_room_ref, room_ref);
    sound_event.direction = direction;
    if party.len() == 1 {
        sound_event.set_verb("walks", "is walking");
        sound_event.set_description(&character_name, "someone", "someone");
    } else {
        sound_event.set_verb("walk", "are walking");
        sound_event.set_description("some people", "people", "people");
    }
    sound_event.excluded_characters = party.clone();
    sound_event
        .excluded_characters
        .append(&mut visual_witnesses.into_iter().collect::<Vec<_>>());
    let mut sound_output = sound_event
        .fire(realm, sound_strength_for_party(&party))
        .ok_or_else(|| "You walked, but nobody heard you.".to_owned())?;

    let mut output = visual_output;
    output.append(&mut sound_output);
    for character in party {
        if character != character_ref {
            output.push(PlayerOutput::new_from_string(
                character.id(),
                format!("You follow {}.\n", character_name),
            ));
        }

        output.append(&mut actions::look_at_entity(realm, character, room_ref)?);
    }
    Ok(output)
}

/// Makes the character leave the given room (on end of session).
pub fn leave_room(
    realm: &mut Realm,
    character_ref: EntityRef,
) -> Result<Vec<PlayerOutput>, String> {
    let character = realm
        .character(character_ref)
        .ok_or("The character doesn't exist.")?;

    let room_ref = character.current_room();
    let character_name = definite_character_name(realm, character_ref)?;

    if let Some(room) = realm.room_mut(room_ref) {
        room.remove_characters(&[character_ref]);
    }

    let mut visual_event = VisualEvent::new(room_ref);
    visual_event.set_description(
        &format!("{} left.", capitalize(&character_name)),
        &format!("You see {} leave.", character_name),
        "You see someone leave.",
    );
    visual_event.excluded_characters.push(character_ref);
    let visual_output = visual_event
        .fire(realm, 1.0)
        .ok_or_else(|| "You left, but nobody saw you.".to_owned())?;

    Ok(visual_output)
}

fn sound_strength_for_party(party: &[EntityRef]) -> f32 {
    let mut strength = 0.0;
    for (i, _) in party.iter().enumerate() {
        let mut added_strength = 1.0 - 0.2 * i as f32;
        if added_strength < 0.3 {
            added_strength = 0.3;
        }
        strength += added_strength;
    }
    strength
}
