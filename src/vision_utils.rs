use std::collections::{HashMap, HashSet};
use std::f64::consts::PI;
use std::hash::{Hash, Hasher};
use std::iter::FromIterator;

use crate::direction_utils::is_direction;
use crate::events::EventType;
use crate::game_object::{Character, GameObject, GameObjectRef};
use crate::objects::{ItemFlags, Portal, Realm, Room, RoomFlags};
use crate::vector3d::Vector3D;
use crate::vector_utils::angle_between_xy_vectors;

const UNDER_QUARTER_PI: f64 = PI / 4.01;
const OVER_QUARTER_PI: f64 = PI / 3.99;
const OVER_THREE_QUARTER_PI: f64 = 3.0 * PI / 3.99;

struct CharacterWithStrengthAndDistance {
    character: GameObjectRef,
    strength: f32,
    distance: i32,
}

impl Eq for CharacterWithStrengthAndDistance {}

impl Hash for CharacterWithStrengthAndDistance {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.character.hash(state)
    }
}

impl PartialEq for CharacterWithStrengthAndDistance {
    fn eq(&self, other: &Self) -> bool {
        self.character == other.character
    }
}

#[derive(Clone, Copy, Debug, Eq, Hash, PartialEq)]
pub enum Position {
    Left,
    Right,
    Ahead,
    Behind,
    Center,
    Above,
    LeftWall,
    RightWall,
    Wall,
    Ceiling,
    Distance,
    Roof,
}

fn characters_visible_through_portal(
    realm: &Realm,
    looking_direction: &Vector3D,
    from_room: &Room,
    origin_room: &Room,
    portal: &Portal,
    strength: f32,
    excluded_rooms: &HashSet<GameObjectRef>,
) -> HashSet<CharacterWithStrengthAndDistance> {
    let room = unwrap_or_return!(
        realm.room(portal.opposite_of(from_room.object_ref())),
        HashSet::new()
    );
    let room_strength = if strength != 0.0 {
        strength
    } else {
        from_room.event_multiplier(EventType::Visual)
    } * portal.event_multiplier(EventType::Visual)
        * room.event_multiplier(EventType::Visual);
    if room_strength < 0.1 {
        return HashSet::new();
    }

    let vector1: Vector3D = room.position() - from_room.position();
    let distance = vector1.len();

    let mut visited_rooms = HashSet::new();
    visited_rooms.insert(from_room.object_ref());
    visited_rooms.insert(room.object_ref());
    for room_ref in excluded_rooms {
        visited_rooms.insert(*room_ref);
    }

    let mut characters = HashSet::from_iter(
        room.characters()
            .iter()
            .filter_map(|character_ref| realm.character(*character_ref))
            .map(|character| CharacterWithStrengthAndDistance {
                character: character.object_ref(),
                strength: room_strength,
                distance,
            }),
    );

    for next_portal in room
        .portals()
        .iter()
        .filter_map(|portal_ref| realm.portal(*portal_ref))
    {
        if !next_portal.can_see_through() {
            continue;
        }

        let next_room = unwrap_or_continue!(realm.room(next_portal.opposite_of(room.object_ref())));
        if next_room.id() == from_room.id() || visited_rooms.contains(&next_room.object_ref()) {
            continue;
        }

        let vector2 = next_room.position() - room.position();
        if room.has_flags(RoomFlags::HasWalls) {
            // If there are walls, we can only follow line-of-sight in a straight line:
            if vector1.x != vector2.x || vector1.y != vector2.y {
                continue;
            }
        } else {
            // In an open area, we only verify the next room is within field of view:
            let vector3 = next_room.position() - origin_room.position();
            let angle = angle_between_xy_vectors(looking_direction, &vector3);
            if angle.abs() > UNDER_QUARTER_PI {
                continue;
            }
        }

        if (room.has_flags(RoomFlags::HasCeiling) && vector2.z > vector1.z)
            || (room.has_flags(RoomFlags::HasFloor) && vector2.z < vector1.z)
        {
            continue;
        }

        for character_with_strength_and_distance in characters_visible_through_portal(
            realm,
            looking_direction,
            &next_room,
            origin_room,
            &next_portal,
            strength,
            &visited_rooms,
        ) {
            characters.insert(character_with_strength_and_distance);
        }
    }

    characters
}

pub fn describe_characters_relative_to(
    realm: &Realm,
    characters: Vec<GameObjectRef>,
    relative: &dyn Character,
) -> String {
    /*TODO: Rustify
    if (!characters || characters.length === 0) {
        return "";
    }

    var groups = {
        "distance": [],
        "ahead": [],
        "roof": []
    };

    var sentences = [];

    var characterInfo;
    for (var i = 0, length = characters.length; i < length; i++) {
        characterInfo = characters[i];
        var flags = characterInfo.character.currentRoom.flags.split("|");
        if (flags.contains("IsRoof")) {
            groups["roof"].push(characterInfo);
        } else if (characterInfo.distance > 50) {
            groups["distance"].push(characterInfo);
        } else {
            groups["ahead"].push(characterInfo);
        }
    }

    for (var key in groups) {
        if (!groups.hasOwnProperty(key) || groups[key].length === 0) {
            continue;
        }

        var group = groups[key];
        for (i = 0; i < group.length; i++) {
            characterInfo = group[i];
            var character = characterInfo.character;
            if (character && character.group && character.group.leader === character) {
                group.removeAt(i);
                character.group.members.forEach(function(member) {
                    for (var i = 0; i < group.length; i++) {
                        if (group[i].character === member) {
                            group.removeAt(i);
                            return;
                        }
                    }
                });
                group.push({
                    "group": character.group,
                    "strength": characterInfo.strength,
                    "distance": characterInfo.distance
                });
                i = 0;
            }
        }

        var prefix = descriptionForGroup(key)[0];

        var infos = [], numMen = 0, numWomen = 0, numUnknown = 0, numPeople = 0;
        for (i = 0; i < group.length; i++) {
            characterInfo = group[i];

            var name, actionDescription;
            if (characterInfo.group) {
                name = characterInfo.group.nameAtStrength(characterInfo.strength);
                if (numPeople > 0 || i < group.length - 1) {
                    name = "a group " + (name.startsWith("a lot ") ? "with " : "of ") +
                                        (name.startsWith("some ") ? name.mid(5) : name);
                }

                actionDescription = describeActionRelativeTo(characterInfo.group.leader,
                                                             relative, characterInfo.distance);

                infos.push({
                    "group": characterInfo.group,
                    "name": name,
                    "action": actionDescription.text
                });
                numPeople += 1 + characterInfo.group.members.length;
            } else {
                if (characterInfo.character.race &&
                    characterInfo.character.race.name === "animal") {
                    continue;
                }

                name = characterInfo.character.nameAtStrength(characterInfo.strength);

                actionDescription = describeActionRelativeTo(characterInfo.character,
                                                             relative, characterInfo.distance);

                if (name === "a man") {
                    numMen++;
                } else if (name === "a woman") {
                    numWomen++;
                } else if (name === "someone") {
                    numUnknown++;
                } else {
                    infos.push({
                        "character": characterInfo.character,
                        "name": name,
                        "action": actionDescription.text
                    });
                }
                numPeople++;
            }

            if (actionDescription.target) {
                for (var j = i + 1; j < group.length; j++) {
                    if (group[j].character === actionDescription.target) {
                        group.removeAt(j);
                        break;
                    }
                }
            }
        }

        for (i = 0; i < infos.length; i++) {
            if (!infos[i].character) {
                continue;
            }

            var count = 1;
            for (j = i + 1; j < infos.length; j++) {
                if (infos[i].name === infos[j].name && infos[i].action === infos[j].action) {
                    infos.removeAt(j);
                    count++;
                    j--;
                }
            }
            if (count > 1) {
                infos[i].name = writtenAmount(count) + " " + infos[i].character.plural;
            }
        }

        var characterTexts = [], hasMan = false, hasWoman = false;
        for (i = 0, length = infos.length; i < length; i++) {
            var info = infos[i];
            name = info.name;
            if (info.action !== "") {
                name += " " + info.action;
            }
            characterTexts.push(name);

            if (info.character) {
                if (info.character.gender === "male") {
                    hasMan = true;
                } else if (info.character.gender === "female") {
                    hasWoman = true;
                }
            } else if (info.group) {
                [info.group.leader].concat(info.group.members).forEach(function(member) {
                    if (member.gender === "male") {
                        hasMan = true;
                    } else if (member.gender === "female") {
                        hasWoman = true;
                    }
                });
            }
        }

        if (numUnknown === 0) {
            if (numMen === 0) {
                if (numWomen === 1) {
                    if (hasWoman) {
                        characterTexts.push("another woman");
                    } else {
                        characterTexts.push("a woman");
                    }
                } else if (numWomen > 1) {
                    if (hasWoman) {
                        characterTexts.push(writtenAmount(numWomen) + " other women");
                    } else {
                        characterTexts.push(writtenAmount(numWomen) + " women");
                    }
                }
            } else if (numMen === 1) {
                if (numWomen <= 1) {
                    if (hasMan) {
                        characterTexts.push("another man");
                    } else {
                        characterTexts.push("a man");
                    }
                    if (numWomen === 1) {
                        if (hasWoman && !hasMan) {
                            characterTexts.push("another woman");
                        } else {
                            characterTexts.push("a woman");
                        }
                    }
                } else {
                    if (hasWoman) {
                        characterTexts.push(writtenAmount(numWomen) + " other women");
                    } else {
                        characterTexts.push(writtenAmount(numWomen) + " women");
                    }
                    if (hasMan && !hasWoman) {
                        characterTexts.push("another man");
                    } else {
                        characterTexts.push("a man");
                    }
                }
            } else {
                if (numWomen <= 1) {
                    if (hasMan) {
                        characterTexts.push(writtenAmount(numMen) + " other men");
                    } else {
                        characterTexts.push(writtenAmount(numMen) + " men");
                    }
                    if (numWomen === 1) {
                        if (hasWoman && !hasMan) {
                            characterTexts.push("another woman");
                        } else {
                            characterTexts.push("a woman");
                        }
                    }
                } else {
                    if (numPeople > numMen + numWomen) {
                        characterTexts.push(writtenAmount(numMen + numWomen) +
                                              " other people");
                    } else {
                        characterTexts.push(writtenAmount(numMen + numWomen) + " people");
                    }
                }
            }
        } else {
            numUnknown += numMen + numWomen;
            if (numPeople > numUnknown) {
                if (numUnknown === 1) {
                    characterTexts.push("someone else");
                } else {
                    characterTexts.push(writtenAmount(numUnknown) + " other people");
                }
            } else {
                if (numUnknown === 1) {
                    characterTexts.push("someone");
                } else {
                    characterTexts.push(writtenAmount(numUnknown) + " people");
                }
            }
        }

        if (!characterTexts.isEmpty()) {
            sentences.push("%1, you see %2.".arg(prefix, Util.joinFancy(characterTexts)));
        }
    }

    return sentences.join(" ");*/
    String::new()
}

pub fn description_for_position(position: Position) -> (&'static str, &'static str, &'static str) {
    match position {
        Position::Left => ("To your left", "is", "are"),
        Position::Right => ("To your right", "is", "are"),
        Position::Ahead => ("Ahead of you, there", "is", "are"),
        Position::Behind => ("Behind you", "is", "are"),
        Position::Above => ("Above you", "is", "are"),
        Position::LeftWall => ("On the left wall", "hangs", "hang"),
        Position::RightWall => ("On the right wall", "hangs", "hang"),
        Position::Wall => ("On the wall", "hangs", "hang"),
        Position::Ceiling => ("From the ceiling", "hangs", "hang"),
        Position::Distance => ("In the distance", "you see", "you see"),
        Position::Roof => ("On the roof", "you see", "you see"),
        _ => ("There", "is", "are"),
    }
}

pub fn group_items_by_position(
    realm: &Realm,
    character: &dyn Character,
    room: &Room,
) -> HashMap<Position, Vec<GameObjectRef>> {
    let mut grouped_items: HashMap<Position, Vec<GameObjectRef>> = HashMap::new();

    for item_ref in room.items() {
        let item = unwrap_or_continue!(realm.item(*item_ref));
        if item.hidden() {
            continue;
        }

        let position = if item.has_flags(ItemFlags::AttachedToCeiling) {
            Position::Ceiling
        } else if item.has_flags(ItemFlags::AttachedToWall) {
            let angle = angle_between_xy_vectors(character.direction(), &item.position().to_vec());
            if item.position().x == 0 && item.position().y == 0
                || angle.abs() > OVER_THREE_QUARTER_PI
                || angle.abs() < UNDER_QUARTER_PI
            {
                Position::Wall
            } else if angle > 0.0 {
                Position::RightWall
            } else {
                Position::LeftWall
            }
        } else if item.position().x == 0 && item.position().y == 0 {
            Position::Center
        } else {
            let angle = angle_between_xy_vectors(character.direction(), &item.position().to_vec());
            if angle.abs() > OVER_THREE_QUARTER_PI {
                Position::Behind
            } else if angle.abs() < UNDER_QUARTER_PI {
                Position::Ahead
            } else if angle > 0.0 {
                Position::Right
            } else {
                Position::Left
            }
        };

        if let Some(items) = grouped_items.get_mut(&position) {
            items.push(*item_ref);
        } else {
            grouped_items.insert(position, vec![*item_ref]);
        }
    }

    grouped_items
}

pub fn group_portals_by_position(
    realm: &Realm,
    character: &dyn Character,
    room: &Room,
) -> HashMap<Position, Vec<GameObjectRef>> {
    let mut grouped_items: HashMap<Position, Vec<GameObjectRef>> = HashMap::new();

    for portal_ref in room.portals() {
        let portal = unwrap_or_continue!(realm.portal(*portal_ref));
        if portal.is_hidden_from_room(room.object_ref()) {
            continue;
        }

        let name = portal.name_from_room(room.object_ref());
        if is_direction(name) || name == "out" {
            continue;
        }

        let vector = portal.position(realm) - room.position();
        let angle = angle_between_xy_vectors(character.direction(), &vector);
        let position = if angle.abs() > OVER_THREE_QUARTER_PI {
            Position::Behind
        } else if angle.abs() < UNDER_QUARTER_PI {
            Position::Ahead
        } else if angle > 0.0 {
            Position::Right
        } else {
            Position::Left
        };

        if let Some(items) = grouped_items.get_mut(&position) {
            items.push(*portal_ref);
        } else {
            grouped_items.insert(position, vec![*portal_ref]);
        }
    }

    grouped_items
}

pub fn visible_characters_through_portals_from_position(
    realm: &Realm,
    room: &Room,
    looking_direction: &Vector3D,
) -> Vec<GameObjectRef> {
    let mut characters = HashSet::new();
    for portal_ref in room.portals() {
        let portal = unwrap_or_continue!(realm.portal(*portal_ref));
        if !portal.can_see_through() || portal.is_hidden_from_room(room.object_ref()) {
            continue;
        }

        let vector = portal.position(realm) - room.position();
        let angle = angle_between_xy_vectors(looking_direction, &vector);
        if angle.abs() < UNDER_QUARTER_PI {
            for character_with_strength_and_distance in characters_visible_through_portal(
                realm,
                looking_direction,
                &room,
                &room,
                &portal,
                room.event_multiplier(EventType::Visual),
                &HashSet::new(),
            ) {
                characters.insert(character_with_strength_and_distance.character);
            }
        }
    }
    characters.into_iter().collect()
}

pub fn visible_items_from_position(
    realm: &Realm,
    room: &Room,
    looking_direction: &Vector3D,
) -> Vec<GameObjectRef> {
    room.items()
        .iter()
        .map(|item_ref| *item_ref)
        .filter(|item_ref| {
            let item = unwrap_or_return!(realm.item(*item_ref), false);
            if item.hidden() {
                return false;
            }

            let is_visible = (item.position().x == 0 && item.position().y == 0)
                || angle_between_xy_vectors(looking_direction, &item.position().to_vec()).abs()
                    < OVER_QUARTER_PI;
            is_visible
        })
        .collect()
}

pub fn visible_portals_from_position(
    realm: &Realm,
    room: &Room,
    looking_direction: &Vector3D,
) -> Vec<GameObjectRef> {
    room.portals()
        .iter()
        .map(|portal_ref| *portal_ref)
        .filter(|portal_ref| {
            let portal = unwrap_or_return!(realm.portal(*portal_ref), false);
            if portal.is_hidden_from_room(room.object_ref()) {
                return false;
            }

            let name = portal.name_from_room(room.object_ref());
            if is_direction(name) || name == "out" {
                return false;
            }

            let vector = portal.position(realm) - room.position();
            let is_visible =
                angle_between_xy_vectors(looking_direction, &vector).abs() < OVER_QUARTER_PI;
            is_visible
        })
        .collect()
}
