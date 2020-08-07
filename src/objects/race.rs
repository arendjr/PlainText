use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;
use std::sync::Arc;

use crate::character_stats::CharacterStats;
use crate::game_object::{GameObject, GameObjectId, GameObjectRef, GameObjectType};

#[derive(Clone, Debug)]
pub struct Race {
    id: GameObjectId,
    adjective: String,
    description: String,
    height: f32,
    name: String,
    revision_num: u32,
    starting_room: GameObjectRef,
    stats: CharacterStats,
    stats_suggestion: CharacterStats,
    weight: f32,
}

impl Race {
    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Arc<dyn GameObject>, String> {
        match serde_json::from_str::<RaceDto>(json) {
            Ok(race_dto) => Ok(Arc::new(Self {
                id,
                adjective: race_dto.adjective,
                description: race_dto.description,
                height: race_dto.height,
                name: race_dto.name,
                revision_num: 0,
                starting_room: race_dto.startingRoom,
                stats: race_dto.stats,
                stats_suggestion: race_dto.statsSuggestion,
                weight: race_dto.weight,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }
}

impl fmt::Display for Race {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Race({}, {})", self.id, self.name)
    }
}

impl GameObject for Race {
    fn get_id(&self) -> GameObjectId {
        self.id
    }

    fn get_object_type(&self) -> GameObjectType {
        GameObjectType::Race
    }

    fn get_name(&self) -> String {
        self.name.clone()
    }

    fn get_revision_num(&self) -> u32 {
        self.revision_num
    }

    fn to_race(&self) -> Option<Self> {
        Some(self.clone())
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct RaceDto {
    adjective: String,
    description: String,
    height: f32,
    name: String,
    startingRoom: GameObjectRef,
    stats: CharacterStats,
    statsSuggestion: CharacterStats,
    weight: f32,
}
