use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::character_stats::CharacterStats;
use crate::game_object::{
    Character, GameObject, GameObjectId, GameObjectRef, GameObjectType, Gender, SharedGameObject,
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
    height: f32,
    name: String,
    race: GameObjectRef,
    session_id: Option<u64>,
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
                height: npc_dto.height,
                name: npc_dto.name,
                race: npc_dto.race,
                session_id: None,
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
                description: "".to_owned(),
                direction: Vector3D::default(),
                gender: Gender::Unspecified,
                height: 0.0,
                name,
                race: race_ref,
                session_id: None,
                stats: CharacterStats::new(),
                weight: 0.0,
            },
            true,
        )
    }
}

impl Character for Npc {
    fn class(&self) -> Option<GameObjectRef> {
        self.class
    }

    fn current_room(&self) -> GameObjectRef {
        self.current_room
    }

    fn direction(&self) -> &Vector3D {
        &self.direction
    }

    fn gender(&self) -> Gender {
        self.gender
    }

    fn race(&self) -> GameObjectRef {
        self.race
    }

    fn stats(&self) -> &CharacterStats {
        &self.stats
    }

    fn with_current_room(&self, room: GameObjectRef) -> (SharedGameObject, bool) {
        (
            SharedGameObject::new(Self {
                current_room: room,
                ..self.clone()
            }),
            true,
        )
    }
}

impl fmt::Display for Npc {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "NPC({}, {})", self.id, self.name)
    }
}

impl GameObject for Npc {
    fn as_character(&self) -> Option<&dyn Character> {
        Some(self)
    }

    fn as_npc(&self) -> Option<&Self> {
        Some(&self)
    }

    fn description(&self) -> &str {
        &self.description
    }

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Player
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn serialize(&self) -> String {
        serde_json::to_string_pretty(&NpcDto {
            class: self.class,
            currentRoom: self.current_room,
            description: if self.description.is_empty() {
                None
            } else {
                Some(self.description.clone())
            },
            direction: Some(self.direction.clone()),
            gender: self.gender.serialize(),
            height: self.height,
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
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct NpcDto {
    class: Option<GameObjectRef>,
    currentRoom: GameObjectRef,
    description: Option<String>,
    direction: Option<Vector3D>,
    gender: Option<String>,
    height: f32,
    name: String,
    race: GameObjectRef,
    stats: CharacterStats,
    weight: f32,
}
