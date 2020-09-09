use crate::game_object::{GameObject, GameObjectRef};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;

pub fn enter(
    mut realm: Realm,
    character_ref: GameObjectRef,
    room_ref: GameObjectRef,
    _: &mut Vec<PlayerOutput>,
) -> Realm {
    if let Some(character) = realm.character(character_ref) {
        let current_room_ref = character.current_room();
        if current_room_ref != room_ref {
            realm = realm.set_shared_object(character_ref, character.with_current_room(room_ref));
        }

        if let Some(current_room) = realm.room(current_room_ref) {
            realm = realm.set(
                current_room.object_ref(),
                current_room.without_characters(vec![character_ref]),
            );
        }
    }

    if let Some(room) = realm.room(room_ref) {
        realm = realm.set(room.object_ref(), room.with_characters(vec![character_ref]));
    }

    // TODO: enteredRoom();

    /* TODO:
    for (const GameObjectPtr &character : room->characters()) {
        if (character != this) {
            character->invokeTrigger("oncharacterentered", this);
        }
    }*/

    realm
}
