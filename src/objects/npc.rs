use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::character_stats::CharacterStats;
use crate::game_object::{
    Character, GameObject, GameObjectId, GameObjectRef, GameObjectType, Gender, SharedGameObject,
};
use crate::vector3d::Vector3D;

use super::Realm;

#[derive(Clone, Debug, PartialEq)]
pub struct Npc {
    id: GameObjectId,
    class: Option<GameObjectRef>,
    current_room: GameObjectRef,
    description: String,
    direction: Vector3D,
    gender: Gender,
    gold: u32,
    height: f32,
    hp: i16,
    inventory: Vec<GameObjectRef>,
    mp: i16,
    name: String,
    race: GameObjectRef,
    stats: CharacterStats,
    weight: f32,
}

impl Npc {
    pub fn hydrate(id: GameObjectId, json: &str) -> Result<SharedGameObject, String> {
        match serde_json::from_str::<NpcDto>(json) {
            Ok(npc_dto) => Ok(SharedGameObject::new(Self {
                id,
                class: npc_dto.class,
                current_room: npc_dto.currentRoom,
                description: npc_dto.description.unwrap_or_default(),
                direction: npc_dto.direction.unwrap_or_default(),
                gender: Gender::hydrate(&npc_dto.gender),
                gold: npc_dto.gold,
                height: npc_dto.height,
                hp: npc_dto.hp,
                inventory: npc_dto.inventory.unwrap_or_default(),
                mp: npc_dto.mp,
                name: npc_dto.name,
                race: npc_dto.race,
                stats: npc_dto.stats,
                weight: npc_dto.weight,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn new(
        id: GameObjectId,
        name: String,
        race_ref: GameObjectRef,
        room_ref: GameObjectRef,
    ) -> (Self, bool) {
        (
            Self {
                id,
                class: None,
                current_room: room_ref,
                description: String::new(),
                direction: Vector3D::default(),
                gender: Gender::Unspecified,
                gold: 0,
                height: 0.0,
                hp: 1,
                inventory: Vec::new(),
                mp: 0,
                name,
                race: race_ref,
                stats: CharacterStats::new(),
                weight: 0.0,
            },
            true,
        )
    }
}

impl Character for Npc {
    game_object_copy_prop!(class, set_class, Option<GameObjectRef>);
    game_object_copy_prop!(current_room, set_current_room, GameObjectRef);
    game_object_ref_prop!(direction, set_direction, Vector3D);
    game_object_copy_prop!(gender, set_gender, Gender);
    game_object_copy_prop!(gold, set_gold, u32);
    game_object_copy_prop!(height, set_height, f32);
    game_object_copy_prop!(hp, set_hp, i16);
    game_object_ref_prop!(inventory, set_inventory, Vec<GameObjectRef>);
    game_object_copy_prop!(mp, set_mp, i16);
    game_object_copy_prop!(race, set_race, GameObjectRef);
    game_object_ref_prop!(stats, set_stats, CharacterStats);
    game_object_copy_prop!(weight, set_weight, f32);
}

impl fmt::Display for Npc {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "NPC({}, {})", self.id, self.name)
    }
}

impl GameObject for Npc {
    game_object_string_prop!(name, set_name);
    game_object_string_prop!(description, set_description);

    fn as_character(&self) -> Option<&dyn Character> {
        Some(self)
    }

    fn as_npc(&self) -> Option<&Self> {
        Some(&self)
    }

    fn dehydrate(&self) -> serde_json::Value {
        serde_json::to_value(NpcDto {
            class: self.class,
            currentRoom: self.current_room,
            description: if self.description.is_empty() {
                None
            } else {
                Some(self.description.clone())
            },
            direction: Some(self.direction.clone()),
            gender: self.gender.serialize(),
            gold: self.gold,
            height: self.height,
            hp: self.hp,
            inventory: if self.inventory.is_empty() {
                None
            } else {
                Some(self.inventory.clone())
            },
            mp: self.mp,
            name: self.name.clone(),
            race: self.race,
            stats: self.stats.clone(),
            weight: self.weight,
        })
        .unwrap_or_else(|error| {
            panic!(
                "Failed to serialize object {:?}: {:?}",
                self.object_ref(),
                error
            )
        })
    }

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Player
    }

    fn set_property(&self, realm: Realm, prop_name: &str, value: &str) -> Result<Realm, String> {
        match prop_name {
            "class" => Ok(self.set_class(realm, Some(GameObjectRef::from_str(value)?))),
            "current_room" => Ok(self.set_current_room(realm, GameObjectRef::from_str(value)?)),
            "description" => Ok(self.set_description(realm, value.to_owned())),
            "direction" => Ok(self.set_direction(realm, Vector3D::from_str(value)?)),
            "gender" => Ok(self.set_gender(realm, Gender::from_str(value)?)),
            "gold" => Ok(self.set_gold(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            "height" => Ok(self.set_height(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            "hp" => Ok(self.set_hp(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            "inventory" => Ok(self.set_inventory(realm, GameObjectRef::vec_from_str(value)?)),
            "mp" => Ok(self.set_mp(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            "name" => Ok(self.set_name(realm, value.to_owned())),
            "race" => Ok(self.set_race(realm, GameObjectRef::from_str(value)?)),
            "stats" => Ok(self.set_stats(realm, CharacterStats::from_str(value)?)),
            "weight" => Ok(self.set_weight(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            _ => Err(format!("No property named \"{}\"", prop_name))?,
        }
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct NpcDto {
    class: Option<GameObjectRef>,
    currentRoom: GameObjectRef,
    description: Option<String>,
    direction: Option<Vector3D>,
    gender: Option<String>,
    gold: u32,
    height: f32,
    hp: i16,
    inventory: Option<Vec<GameObjectRef>>,
    mp: i16,
    name: String,
    race: GameObjectRef,
    stats: CharacterStats,
    weight: f32,
}
