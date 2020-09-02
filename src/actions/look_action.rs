use crate::colors::Color;
use crate::game_object::{GameObject, GameObjectRef, GameObjectType};
use crate::objects::{Realm, Room};
use crate::player_output::PlayerOutput;
use crate::text_utils::colorize;

/**
 * Look at the given object from the player's perspective.
 */
pub fn look(
    realm: Realm,
    player_ref: GameObjectRef,
    object_ref: GameObjectRef,
    output: &mut Vec<PlayerOutput>,
) -> Realm {
    match realm.object(object_ref) {
        Some(object) => match object.object_type() {
            GameObjectType::Room => {
                look_at_room(realm, player_ref, object.as_room().unwrap(), output)
            }
            _ => {
                let description = object.description();
                output.push(PlayerOutput::new_from_string(
                    player_ref.id(),
                    if description.is_empty() {
                        format!("There is nothing special about the {}.", object.name())
                    } else {
                        description.to_owned()
                    },
                ));
                realm
            }
        },
        None => realm,
    }
}

fn look_at_room(
    realm: Realm,
    player_ref: GameObjectRef,
    room: &Room,
    output: &mut Vec<PlayerOutput>,
) -> Realm {
    output.push(PlayerOutput::new_from_string(
        player_ref.id(),
        format!(
            "{}{}",
            if room.name().is_empty() {
                "".to_owned()
            } else {
                format!("\n{}\n\n", colorize(room.name(), Color::Teal))
            },
            room.description(),
        ),
    ));

    /* TODO: var flags = this.flags.split("|");
    var hasDynamicPortals = !flags.contains("OmitDynamicPortalsFromDescription");
    var hasDistantCharacters = !flags.contains("OmitDistantCharactersFromDescription");

    var itemGroups = VisualUtil.divideItemsIntoGroups(this.items, character.direction);

    var portalGroups;
    if (hasDynamicPortals || hasDistantCharacters) {
        portalGroups = VisualUtil.dividePortalsAndCharactersIntoGroups(character, this);
    }

    var itemTexts = [];
    for (var key in itemGroups) {
        if (!itemGroups[key].isEmpty() || hasDynamicPortals && !portalGroups[key].isEmpty()) {
            var itemGroup = itemGroups[key];
            var plural = itemGroup.firstItemIsPlural();

            var combinedItems = Util.combinePtrList(itemGroup);

            if (hasDynamicPortals && key !== "characters") {
                portalGroups[key].forEach(function(portal) {
                    combinedItems.append(portal.nameWithDestinationFromRoom(self));
                });
            }

            var groupDescription = VisualUtil.descriptionForGroup(key);
            var prefix = groupDescription[0];
            var helperVerb = groupDescription[plural ? 2 : 1];
            itemTexts.append("%1 %2 %3.".arg(prefix, helperVerb, Util.joinFancy(combinedItems))
                                        .replace("there is", "there's"));
        }
    }
    if (!itemTexts.isEmpty()) {
        if (!text.endsWith(" ") && !text.endsWith("\n")) {
            text += " ";
        }
        text += itemTexts.join(" ");
    }

    var characterText = "";
    if (hasDistantCharacters && portalGroups.hasOwnProperty("characters")) {
        var characters = portalGroups["characters"];
        characterText = VisualUtil.describeCharactersRelativeTo(characters, character);
    }

    if (!characterText.isEmpty()) {
        if (!text.endsWith(" ") && !text.endsWith("\n")) {
            text += " ";
        }
        text += characterText;
    }
    text += "\n";

    var exitNames = [];
    this.portals.forEach(function(portal) {
        if (!portal.isHiddenFromRoom(self)) {
            exitNames.append(portal.nameFromRoom(self));
        }
    });
    if (!exitNames.isEmpty()) {
        exitNames = Util.sortExitNames(exitNames);
        text += ("Obvious exits: " + exitNames.join(", ") + ".").colorized(Color.Green) + "\n";
    }

    var others = this.characters;
    others.removeOne(character);
    if (!others.isEmpty()) {
        text += "You see %1.\n".arg(others.joinFancy());
    }

    return text;*/
    realm
}
