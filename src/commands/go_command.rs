use crate::actions::enter;
use crate::game_object::GameObject;
use crate::objects::{Player, Realm};
use crate::player_output::PlayerOutput;

/// Makes the character travel to another room.
///
/// # Arguments
///
/// * `where_to` - Name of a portal to travel through
pub fn go(mut realm: Realm, player: &Player, where_to: String) -> (Realm, Vec<PlayerOutput>) {
    let current_room = unwrap_or_return!(realm.room(player.current_room()), (realm, vec![]));

    let portal = current_room
        .portals()
        .iter()
        .filter_map(|portal| realm.portal(*portal))
        .find(|portal| portal.name_from_room(current_room.object_ref()) == where_to);
    let room = portal
        .map(|portal| portal.opposite_of(current_room.object_ref()))
        .and_then(|room_ref| realm.room(room_ref));

    let mut output = Vec::new();
    if let Some(room) = room {
        realm = enter(realm, player.object_ref(), room.object_ref(), &mut output);
    }
    (realm, output)
}
