use std::sync::Arc;

use crate::game_object::{GameObject, GameObjectRef};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;

pub fn enter(
    mut realm: Realm,
    player_ref: GameObjectRef,
    room: GameObjectRef,
    _: &mut Vec<PlayerOutput>,
) -> Realm {
    if let Some(mut player) = realm.get_player(player_ref.id()) {
        if player.current_room() != room {
            player.set_current_room(room);
            realm = realm.set(player_ref, Arc::new(player));
        }
    }

    if let Some(room) = realm.get_room(room.id()) {
        realm = realm.set(
            room.object_ref(),
            Arc::new(room.with_characters(vec![player_ref])),
        );
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
