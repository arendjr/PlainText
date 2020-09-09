use crate::colors::Color;
use crate::game_object::{GameObject, GameObjectRef, GameObjectType};
use crate::objects::{Realm, Room, RoomFlags};
use crate::player_output::PlayerOutput;
use crate::text_utils::{colorize, describe_items, first_item_is_plural, join_sentence};
use crate::visual_utils::{
    description_for_position, group_items_by_position, group_portals_by_position,
};

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
    let mut text = format!(
        "{}{}",
        if room.name().is_empty() {
            "".to_owned()
        } else {
            format!("\n{}\n\n", colorize(room.name(), Color::Teal))
        },
        room.description(),
    );

    let item_text = create_item_texts(&realm, player_ref, room);
    if !item_text.is_empty() {
        if !text.ends_with(" ") && !text.ends_with("\n") {
            text.push(' ');
        }
        text.push_str(&item_text);
    }

    output.push(PlayerOutput::new_from_string(player_ref.id(), text));

    /* var characterText = "";
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
    };

    return text;*/
    realm
}

fn create_item_texts(realm: &Realm, player_ref: GameObjectRef, room: &Room) -> String {
    let mut grouped_items = group_items_by_position(realm, player_ref, room.object_ref());
    let has_dynamic_portals = room.has_flags(RoomFlags::DynamicPortalDescriptions);
    if has_dynamic_portals {
        for (position, portals) in group_portals_by_position(realm, player_ref, room.object_ref()) {
            if let Some(items) = grouped_items.get_mut(&position) {
                for portal in portals {
                    items.push(portal);
                }
            } else {
                grouped_items.insert(position, portals);
            }
        }
    }

    grouped_items
        .iter()
        .map(|(position, item_refs)| {
            let (prefix, singular_verb, plural_verb) = description_for_position(*position);
            let verb = if first_item_is_plural(realm, item_refs) {
                plural_verb
            } else {
                singular_verb
            };

            let item_descriptions = describe_items(realm, item_refs);

            format!(
                "{} {} {}.",
                prefix,
                verb,
                join_sentence(
                    item_descriptions
                        .iter()
                        .map(|string| string.as_str())
                        .collect()
                )
            )
            .replace("there is", "there's")
        })
        .fold("".to_owned(), |result, string| {
            if result.is_empty() {
                string
            } else if result.ends_with(" ") || result.ends_with("\n") {
                result + &string
            } else {
                format!("{} {}", result, string)
            }
        })
}
