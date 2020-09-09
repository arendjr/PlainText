use crate::game_object::{GameObject, GameObjectRef};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;

pub fn enter(
    mut realm: Realm,
    player_ref: GameObjectRef,
    room: GameObjectRef,
    _: &mut Vec<PlayerOutput>,
) -> Realm {
    if let Some(player) = realm.player(player_ref) {
        if player.current_room() != room {
            realm = realm.set(player_ref, player.with_current_room(room));
        }
    }

    if let Some(room) = realm.room(room) {
        realm = realm.set(room.object_ref(), room.with_characters(vec![player_ref]));
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
