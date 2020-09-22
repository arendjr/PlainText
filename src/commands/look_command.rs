use crate::actions::look as look_action;
use crate::game_object::{Character, GameObjectRef};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::sessions::SessionOutput;

use super::CommandLineProcessor;

/// Makes the character look at *something*.
///
/// Examples: `look`, `look at door`, `look earl`, `examine sign`, `look at key in inventory`
pub fn look(
    mut realm: Realm,
    player_ref: GameObjectRef,
    mut processor: CommandLineProcessor,
) -> (Realm, Vec<PlayerOutput>) {
    let mut output = Vec::new();
    let player = unwrap_or_return!(realm.player(player_ref), (realm, output));
    let current_room = unwrap_or_return!(realm.room(player.current_room()), (realm, output));

    let alias = processor.take_word().unwrap();

    if alias.starts_with('l') {
        if !processor.has_words_left() {
            let room_ref = player.current_room();
            realm = look_action(realm, player_ref, room_ref, &mut output);
            return (realm, output);
        }

        processor.skip_connecting_word("at");
    }

    if processor.peek_rest() == "inventory" || processor.peek_rest() == "in inventory" {
        /* TODO: execute inventory action
        player.execute("inventory");
        return;*/
    }

    let description = match processor.take_object_description() {
        Some(description) => description,
        None => {
            output.push(PlayerOutput::new(
                player_ref.id(),
                SessionOutput::Str(if alias == "examine" {
                    "Examine what?"
                } else {
                    "Look at what?"
                }),
            ));
            return (realm, output);
        }
    };

    if processor.peek_rest() == "in inventory" {
        if let Some(object_ref) =
            processor.object_by_description(&realm, player.inventory(), description)
        {
            realm = look_action(realm, player_ref, object_ref, &mut output);
        } else {
            output.push(PlayerOutput::new(
                player_ref.id(),
                SessionOutput::Str("You don't have that."),
            ));
        }
        return (realm, output);
    }

    let pool = [
        &current_room.characters()[..],
        &current_room.items()[..],
        &current_room.portals()[..],
        &player.inventory()[..],
    ]
    .concat();

    let maybe_object_ref = processor.object_by_description(&realm, &pool, description);

    /*
    TODO: There is more...
    if (!object) {
        if (Util.isDirectionAbbreviation(description.name)) {
            description.name = Util.direction(description.name);
        }
        if (Util.isDirection(description.name)) {
            player.send("You look %1.".arg(description.name));
            player.direction = Util.vectorForDirection(description.name);
            player.lookAhead();
        } else if (description.name === "left") {
            player.send("You look to the left.");
            player.direction = [player.direction[1], -player.direction[0], player.direction[2]];
            player.lookAhead();
        } else if (description.name === "right") {
            player.send("You look to the right.");
            player.direction = [-player.direction[1], player.direction[0], player.direction[2]];
            player.lookAhead();
        } else if (description.name === "back" || description.name === "behind") {
            player.send("You look behind you.");
            player.direction = [-player.direction[0], -player.direction[1], -player.direction[2]];
            player.lookAhead();
        } else {
            player.send("Look where?");
        }
        return;
    }

    player.send(object.lookAtBy(player));

    function describeNearbyObject(nearbyObject, vector, position) {
        if (vector.angle(position) < Math.PI / 8) {
            var angle = Util.angleBetweenXYVectors(vector, position);

            if (nearbyObject.isPortal()) {
                player.send("On its %1 is %2.".arg(angle > 0 ? "right" : "left",
                                                   nearbyObject.nameWithDestinationFromRoom(room)));
            } else {
                player.send("On its %1 there's %2.".arg(angle > 0 ? "right" : "left",
                                                        nearbyObject.indefiniteName()));
            }
        }
    }

    var showNearbyObjects = (object.isItem() && !object.position.equals([0, 0, 0])) ||
                            object.isPortal();
    if (showNearbyObjects) {
        var vector = (object.isPortal() ? object.position.minus(room.position) : object.position);
        room.items.forEach(function(item) {
            if (item !== object) {
                describeNearbyObject(item, vector, item.position);
            }
        });
        room.portals.forEach(function(portal) {
            if (portal !== object) {
                describeNearbyObject(portal, vector, portal.position.minus(room.position));
            }
        });
    }

    if (object.isPortal() && object.canSeeThrough()) {
        var strength = room.eventMultiplier("Visual");
        if (player.weapon && player.weapon.name === "binocular") {
            strength *= 4;
        }

        var characters = VisualUtil.charactersVisibleThroughPortal(player, room, object, strength);
        var characterText = VisualUtil.describeCharactersRelativeTo(characters, player);
        if (characterText !== "") {
            player.send(characterText);
        }
    }
    */

    if let Some(object_ref) = maybe_object_ref {
        realm = look_action(realm, player_ref, object_ref, &mut output);
    } else {
        output.push(PlayerOutput::new(
            player_ref.id(),
            SessionOutput::Str(if alias == "examine" {
                "Examine what?"
            } else {
                "Look where?"
            }),
        ));
    }

    (realm, output)
}
