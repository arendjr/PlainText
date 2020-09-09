use crate::actions::enter;
use crate::game_object::{Character, GameObject, GameObjectRef};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;

/// Makes the character travel to another room.
///
/// # Arguments
///
/// * `where_to` - Name of a portal to travel through
pub fn go(
    mut realm: Realm,
    player_ref: GameObjectRef,
    where_to: String,
) -> (Realm, Vec<PlayerOutput>) {
    let mut output = Vec::new();
    let player = unwrap_or_return!(realm.player(player_ref), (realm, output));
    let current_room = unwrap_or_return!(realm.room(player.current_room()), (realm, output));

    let portal = current_room
        .portals()
        .iter()
        .filter_map(|portal| realm.portal(*portal))
        .find(|portal| portal.name_from_room(current_room.object_ref()) == where_to);
    let maybe_room = portal
        .map(|portal| portal.opposite_of(current_room.object_ref()))
        .and_then(|room_ref| realm.room(room_ref));

    if let Some(room) = maybe_room {
        let room_ref = room.object_ref();
        realm = enter(realm, player_ref, room_ref, &mut output);
    }
    (realm, output)
}
