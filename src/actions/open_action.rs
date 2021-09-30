use crate::{
    game_object::{GameObject, GameObjectRef},
    objects::{PortalFlags, Realm},
    player_output::PlayerOutput,
    text_utils::capitalize,
};

/// Opens a portal.
pub fn open(
    realm: &mut Realm,
    character_ref: GameObjectRef,
    portal_ref: GameObjectRef,
) -> Result<Vec<PlayerOutput>, String> {
    let (character, room) = realm.character_and_room_res(character_ref)?;
    let portal = realm.portal(portal_ref).ok_or("That exit doesn't exist.")?;

    if !portal.can_open_from_room(room.object_ref()) {
        return Err("Exit cannot be opened.".into());
    }

    let name = portal.name_from_room(room.object_ref());
    if portal.is_open() {
        return Err(format!("The {} is already open.", name));
    }

    let mut output = vec![];
    output.push(PlayerOutput::new_from_string(
        character.id(),
        format!("You open the {}.\n", name),
    ));

    let character_name = capitalize(&character.definite_name(realm)?);
    for character in room.characters() {
        if *character != character_ref {
            output.push(PlayerOutput::new_from_string(
                character.id(),
                format!("{} opens the {}.\n", character_name, name),
            ));
        }
    }

    if let Some(portal) = realm.portal_mut(portal_ref) {
        portal.set_flags(portal.flags() | PortalFlags::IsOpen);
    }

    Ok(output)
}

/// Closes a portal.
pub fn close(
    realm: &mut Realm,
    character_ref: GameObjectRef,
    portal_ref: GameObjectRef,
) -> Result<Vec<PlayerOutput>, String> {
    let (character, room) = realm.character_and_room_res(character_ref)?;
    let portal = realm.portal(portal_ref).ok_or("That exit doesn't exist.")?;

    if !portal.can_open_from_room(room.object_ref()) {
        return Err("Exit cannot be closed.".into());
    }

    let name = portal.name_from_room(room.object_ref());
    if !portal.is_open() {
        return Err(format!("The {} is already closed.", name));
    }

    let mut output = vec![];
    output.push(PlayerOutput::new_from_string(
        character.id(),
        format!("You close the {}.\n", name),
    ));

    let character_name = capitalize(&character.definite_name(realm)?);
    for character in room.characters() {
        if *character != character_ref {
            output.push(PlayerOutput::new_from_string(
                character.id(),
                format!("{} closes the {}.\n", character_name, name),
            ));
        }
    }

    if let Some(portal) = realm.portal_mut(portal_ref) {
        portal.set_flags(portal.flags() & !PortalFlags::IsOpen);
    }

    Ok(output)
}
