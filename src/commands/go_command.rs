use crate::actions::enter;
use crate::game_object::GameObject;
use crate::objects::{Player, Realm};
use crate::player_output::PlayerOutput;

/// Makes the character travel to another room.
///
/// # Arguments
///
/// * `where_to` - Name of a portal to travel through
pub fn go(mut realm: Realm, player: Player, where_to: String) -> (Realm, Vec<PlayerOutput>) {
    let current_room = match realm.get_room(player.current_room().id()) {
        Some(room) => room,
        None => return (realm, vec![]),
    };

    let portal = current_room
        .portals()
        .iter()
        .filter_map(|portal| realm.get_portal(portal.id()))
        .find(|portal| portal.name_from_room(current_room.object_ref()) == where_to);
    let room = portal
        .map(|portal| portal.opposite_of(current_room.object_ref()))
        .and_then(|room| realm.get_room(room.id()));

    let mut output = Vec::new();
    if let Some(room) = room {
        realm = enter(realm, player.object_ref(), room.object_ref(), &mut output);
    }
    (realm, output)
}
