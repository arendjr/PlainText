use std::sync::Arc;

use crate::game_object::{GameObject, GameObjectRef};
use crate::objects::{Player, Realm};
use crate::player_output::PlayerOutput;

pub fn enter(
    mut realm: Realm,
    mut player: Player,
    room: GameObjectRef,
) -> (Realm, Vec<PlayerOutput>) {
    let player_ref = player.get_ref();
    if player.get_current_room() != room {
        player.set_current_room(room);
        realm = realm.set(player_ref, Arc::new(player));
    }

    if let Some(room) = realm.get_room(room.get_id()) {
        realm = realm.set(
            room.get_ref(),
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

    (realm, vec![])
}
