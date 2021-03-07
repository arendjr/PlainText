use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;

use super::change_direction;

/// Makes the character enter the given portal.
pub fn enter_portal(
    realm: Realm,
    character_ref: GameObjectRef,
    portal_ref: GameObjectRef,
    from_room_ref: GameObjectRef,
    output: &mut Vec<PlayerOutput>,
) -> Realm {
    let portal = unwrap_or_return!(realm.portal(portal_ref), realm);
    let target_room_ref = portal.opposite_of(from_room_ref);
    enter_room(realm, character_ref, target_room_ref, output)
}

/// Makes the character enter the given room.
pub fn enter_room(
    mut realm: Realm,
    character_ref: GameObjectRef,
    room_ref: GameObjectRef,
    output: &mut Vec<PlayerOutput>,
) -> Realm {
    if let Some(character) = realm.character(character_ref) {
        let current_room_ref = character.current_room();
        if current_room_ref != room_ref {
            realm = character.set_current_room(realm, room_ref);

            if let (Some(current_room), Some(new_room)) =
                (realm.room(current_room_ref), realm.room(room_ref))
            {
                let direction = new_room.position() - current_room.position();
                realm = change_direction(realm, character_ref, direction, output);
            }
        }

        if let Some(current_room) = realm.room(current_room_ref) {
            realm = current_room.remove_characters(realm, vec![character_ref]);
        }
    }

    if let Some(room) = realm.room(room_ref) {
        realm = room.add_characters(realm, vec![character_ref]);
    }

    // TODO: enteredRoom();

    /* TODO:
    for (const GameObjectPtr &character : room->characters()) {
        if (character != this) {
            character->invokeTrigger("onCharacterEntered", this);
        }
    }*/

    realm
}
