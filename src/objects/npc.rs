use serde::{Deserialize, Serialize};
use std::fmt;

use crate::character_stats::CharacterStats;
use crate::game_object::{
    Character, GameObject, GameObjectId, GameObjectRef, GameObjectType, Gender,
};
use crate::vector3d::Vector3D;

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
    indefinite_article: String,
    inventory: Vec<GameObjectRef>,
    mp: i16,
    name: String,
    needs_sync: bool,
    plural_form: String,
    race: GameObjectRef,
    stats: CharacterStats,
    weight: f32,
}

impl Npc {
    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Box<dyn GameObject>, String> {
        match serde_json::from_str::<NpcDto>(json) {
            Ok(npc_dto) => Ok(Box::new(Self {
                id,
                class: npc_dto.class,
                current_room: npc_dto.currentRoom,
                description: npc_dto.description.unwrap_or_default(),
                direction: npc_dto.direction.unwrap_or_default(),
                gender: Gender::hydrate(&npc_dto.gender),
                gold: npc_dto.gold,
                height: npc_dto.height,
                hp: npc_dto.hp,
                indefinite_article: npc_dto.indefiniteArticle.unwrap_or_default(),
                inventory: npc_dto.inventory.unwrap_or_default(),
                mp: npc_dto.mp,
                name: npc_dto.name,
                needs_sync: false,
                plural_form: npc_dto.plural.unwrap_or_default(),
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
    ) -> Self {
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
            indefinite_article: String::new(),
            inventory: Vec::new(),
            mp: 0,
            name,
            needs_sync: true,
            plural_form: String::new(),
            race: race_ref,
            stats: CharacterStats::new(),
            weight: 0.0,
        }
    }

    pub fn set_indefinite_article(&mut self, indefinite_article: String) {
        self.indefinite_article = indefinite_article;
    }

    pub fn set_plural_form(&mut self, plural_form: String) {
        self.plural_form = plural_form;
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

    fn as_character_mut(&mut self) -> Option<&mut dyn Character> {
        Some(self)
    }

    fn as_npc(&self) -> Option<&Self> {
        Some(&self)
    }

    fn as_npc_mut(&mut self) -> Option<&mut Self> {
        Some(self)
    }

    fn as_object(&self) -> Option<&dyn GameObject> {
        Some(self)
    }

    fn as_object_mut(&mut self) -> Option<&mut dyn GameObject> {
        Some(self)
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
            indefiniteArticle: if self.indefinite_article.is_empty() {
                None
            } else {
                Some(self.indefinite_article.clone())
            },
            inventory: if self.inventory.is_empty() {
                None
            } else {
                Some(self.inventory.clone())
            },
            mp: self.mp,
            name: self.name.clone(),
            plural: if self.plural_form.is_empty() {
                None
            } else {
                Some(self.plural_form.clone())
            },
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

    fn indefinite_article(&self) -> &str {
        &self.indefinite_article
    }

    fn needs_sync(&self) -> bool {
        self.needs_sync
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Player
    }

    fn plural_form(&self) -> &str {
        &self.plural_form
    }

    fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "class" => self.set_class(Some(GameObjectRef::from_str(value)?)),
            "currentRoom" => self.set_current_room(GameObjectRef::from_str(value)?),
            "description" => self.set_description(value.to_owned()),
            "direction" => self.set_direction(Vector3D::from_str(value)?),
            "gender" => self.set_gender(Gender::from_str(value)?),
            "gold" => self.set_gold(value.parse().map_err(|error| format!("{:?}", error))?),
            "height" => self.set_height(value.parse().map_err(|error| format!("{:?}", error))?),
            "hp" => self.set_hp(value.parse().map_err(|error| format!("{:?}", error))?),
            "indefiniteArticle" => self.set_indefinite_article(value.to_owned()),
            "inventory" => self.set_inventory(GameObjectRef::vec_from_str(value)?),
            "mp" => self.set_mp(value.parse().map_err(|error| format!("{:?}", error))?),
            "name" => self.set_name(value.to_owned()),
            "pluralForm" => self.set_plural_form(value.to_owned()),
            "race" => self.set_race(GameObjectRef::from_str(value)?),
            "stats" => self.set_stats(CharacterStats::from_str(value)?),
            "weight" => self.set_weight(value.parse().map_err(|error| format!("{:?}", error))?),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct NpcDto {
    #[serde(skip_serializing_if = "Option::is_none")]
    class: Option<GameObjectRef>,
    currentRoom: GameObjectRef,
    #[serde(skip_serializing_if = "Option::is_none")]
    description: Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    direction: Option<Vector3D>,
    #[serde(skip_serializing_if = "Option::is_none")]
    gender: Option<String>,
    gold: u32,
    height: f32,
    hp: i16,
    #[serde(skip_serializing_if = "Option::is_none")]
    indefiniteArticle: Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    inventory: Option<Vec<GameObjectRef>>,
    mp: i16,
    name: String,
    #[serde(skip_serializing_if = "Option::is_none")]
    plural: Option<String>,
    race: GameObjectRef,
    stats: CharacterStats,
    weight: f32,
}
