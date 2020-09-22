use crate::actions::{enter, look};
use crate::game_object::{Character, GameObject, GameObjectRef};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::sessions::SessionOutput;

use super::CommandLineProcessor;

/// Makes the character travel to another room.
///
/// Examples: `go north`, `go to tower`, `enter door`, `go forward`
pub fn go(
    mut realm: Realm,
    player_ref: GameObjectRef,
    mut processor: CommandLineProcessor,
) -> (Realm, Vec<PlayerOutput>) {
    let mut output = Vec::new();
    let player = unwrap_or_return!(realm.player(player_ref), (realm, output));
    let current_room = unwrap_or_return!(realm.room(player.current_room()), (realm, output));

    let alias = processor.take_word().unwrap();

    if alias == "go" {
        if let Some(direction) = processor.peek_word() {
            if direction == "forward"
                || direction == "back"
                || direction == "left"
                || direction == "right"
            {
                // TODO: Adjust direction using action
            }
        }

        processor.skip_connecting_word("to");
    }

    let maybe_portal = processor
        .take_object(&realm, current_room.portals())
        .and_then(|portal_ref| realm.portal(portal_ref));
    let maybe_room = maybe_portal
        .map(|portal| portal.opposite_of(current_room.object_ref()))
        .and_then(|room_ref| realm.room(room_ref));

    if let Some(room) = maybe_room {
        let room_ref = room.object_ref();
        realm = enter(realm, player_ref, room_ref, &mut output);
        realm = look(realm, player_ref, room_ref, &mut output);
    } else {
        output.push(PlayerOutput::new(
            player_ref.id(),
            SessionOutput::Str("Go where?"),
        ));
    }

    (realm, output)
}
