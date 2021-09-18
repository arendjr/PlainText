use std::collections::BTreeSet;

use crate::events::{AudibleMovementEvent, VisualMovementEvent};
use crate::game_object::{GameObjectRef, GameObjectType};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::vector3d::Vector3D;

use super::change_direction;

/// Makes the character enter the given portal.
pub fn enter_portal(
    realm: &mut Realm,
    character_ref: GameObjectRef,
    portal_ref: GameObjectRef,
    from_room_ref: GameObjectRef,
) -> Result<Vec<PlayerOutput>, String> {
    let portal = realm
        .portal(portal_ref)
        .ok_or_else(|| "That portal doesn't exist.".to_owned())?;

    let target_room_ref = portal.opposite_of(from_room_ref);
    enter_room(realm, character_ref, target_room_ref)
}

/// Makes the character enter the given room.
pub fn enter_room(
    realm: &mut Realm,
    character_ref: GameObjectRef,
    room_ref: GameObjectRef,
) -> Result<Vec<PlayerOutput>, String> {
    let character = realm
        .character_mut(character_ref)
        .ok_or("The character doesn't exist.")?;

    let current_room_ref = character.current_room();
    if current_room_ref == room_ref {
        return Ok(vec![]); // Nothing to do.
    }

    character.set_current_room(room_ref);
    let character_name = character.name().to_owned();

    let direction = match (realm.room(current_room_ref), realm.room(room_ref)) {
        (Some(current_room), Some(new_room)) => {
            let direction = (new_room.position() - current_room.position()).normalized();
            change_direction(realm, character_ref, direction.clone());
            direction
        }
        (_, _) => Vector3D::default(),
    };

    if let Some(current_room) = realm.room_mut(current_room_ref) {
        current_room.remove_characters(vec![character_ref]);
    }

    if let Some(room) = realm.room_mut(room_ref) {
        room.add_characters(vec![character_ref]);
    }

    let mut visual_event = VisualMovementEvent::new(character_ref, current_room_ref, room_ref);
    visual_event.direction = direction.clone();
    visual_event.set_verb("walks", "is walking");
    visual_event.excluded_characters = vec![character_ref];
    let visual_output = visual_event
        .fire(realm, 1.0)
        .ok_or_else(|| "You walked, but nobody saw you.".to_owned())?;

    let visual_witnesses = visual_output
        .iter()
        .map(|output| GameObjectRef(GameObjectType::Player, output.player_id))
        .collect::<BTreeSet<_>>();

    let mut sound_event = AudibleMovementEvent::new(character_ref, current_room_ref, room_ref);
    sound_event.direction = direction;
    sound_event.set_verb("walks", "is walking");
    sound_event.set_description(&character_name, "someone", "someone");
    sound_event
        .excluded_characters
        .append(&mut visual_witnesses.into_iter().collect::<Vec<_>>());
    let mut sound_output = sound_event
        .fire(realm, 1.0)
        .ok_or_else(|| "You walked, but nobody heard you.".to_owned())?;

    let mut output = visual_output;
    output.append(&mut sound_output);
    Ok(output)
}
