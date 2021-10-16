use crate::{
    actionable_events::{ActionDispatcher, ActionableEvent},
    entity::{Entity, EntityRef, Realm},
    player_output::PlayerOutput,
    text_utils::{capitalize, definite_character_name},
};
use std::time::Duration;

/// Opens a portal.
pub fn open(
    realm: &mut Realm,
    action_dispatcher: &ActionDispatcher,
    character_ref: EntityRef,
    portal_ref: EntityRef,
) -> Result<Vec<PlayerOutput>, String> {
    let (character, room) = realm.character_and_room_res(character_ref)?;
    let portal = realm.portal(portal_ref).ok_or("That exit doesn't exist.")?;

    if !portal.can_open_from_room(room.entity_ref()) {
        return Err("Exit cannot be opened.".into());
    }

    let name = portal.name_from_room(room.entity_ref());
    if portal.is_open() {
        return Err(format!("The {} is already open.", name));
    }

    let mut output = vec![];

    if let Some(openable) = portal.as_openable() {
        let required_key = openable.required_key();
        if required_key.is_empty() {
            output.push(PlayerOutput::new_from_string(
                character_ref.id(),
                format!("You open the {}.\n", name),
            ));
        } else if character
            .inventory()
            .iter()
            .any(|item| match realm.item(*item) {
                Some(item) => item.name() == required_key,
                _ => false,
            })
        {
            output.push(PlayerOutput::new_from_string(
                character_ref.id(),
                format!("You use the {} and open the {}.\n", required_key, name),
            ));
        } else {
            return Err(format!("The {} is locked.", name));
        }
    }

    let character_name = capitalize(&definite_character_name(realm, character_ref)?);
    for character in room.characters() {
        if *character != character_ref {
            output.push(PlayerOutput::new_from_string(
                character.id(),
                format!("{} opens the {}.\n", character_name, name),
            ));
        }
    }

    if let Some(openable) = realm.openable_mut(portal_ref) {
        openable.set_open(true);
    }

    if let Some(openable) = realm.openable(portal_ref) {
        if let Some(timeout) = openable.auto_close_timeout() {
            action_dispatcher.dispatch_after(
                ActionableEvent::AutoClose {
                    entity: portal_ref,
                    message: openable.auto_close_message().to_owned(),
                },
                Duration::from_millis(timeout.get() as u64),
            )
        }
    }

    Ok(output)
}

/// Closes a portal.
pub fn close(
    realm: &mut Realm,
    character_ref: EntityRef,
    portal_ref: EntityRef,
) -> Result<Vec<PlayerOutput>, String> {
    let (_, room) = realm.character_and_room_res(character_ref)?;
    let portal = realm.portal(portal_ref).ok_or("That exit doesn't exist.")?;

    if !portal.can_open_from_room(room.entity_ref()) {
        return Err("Exit cannot be closed.".into());
    }

    let name = portal.name_from_room(room.entity_ref());
    if !portal.is_open() {
        return Err(format!("The {} is already closed.", name));
    }

    let mut output = vec![PlayerOutput::new_from_string(
        character_ref.id(),
        format!("You close the {}.\n", name),
    )];

    let character_name = capitalize(&definite_character_name(realm, character_ref)?);
    for character in room.characters() {
        if *character != character_ref {
            output.push(PlayerOutput::new_from_string(
                character.id(),
                format!("{} closes the {}.\n", character_name, name),
            ));
        }
    }

    if let Some(portal) = realm.portal_mut(portal_ref) {
        portal.set_open(false);
    }

    Ok(output)
}
