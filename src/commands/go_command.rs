use crate::actions::enter;
use crate::game_object::GameObject;
use crate::objects::{Player, Realm};
use crate::player_output::PlayerOutput;

/// Makes the character travel to another room.
///
/// # Arguments
///
/// * `where_to` - Name of a portal to travel through
pub fn go(realm: Realm, player: Player, where_to: String) -> (Realm, Vec<PlayerOutput>) {
    let current_room = match realm.get_room(player.get_current_room().get_id()) {
        Some(room) => room,
        None => return (realm, vec![]),
    };

    let portal = current_room
        .portals()
        .iter()
        .filter_map(|portal| realm.get_portal(portal.get_id()))
        .find(|portal| portal.get_name_from_room(current_room.get_ref()) == where_to);
    let room = portal
        .map(|portal| portal.opposite_of(current_room.get_ref()))
        .and_then(|room| realm.get_room(room.get_id()));

    if let Some(room) = room {
        enter(realm, player, room.get_ref())
    } else {
        (realm, vec![])
    }
}
