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
    classes: Arc<Vec<GameObjectRef>>,
    description: String,
    height: f32,
    name: String,
    starting_room: GameObjectRef,
    stats: CharacterStats,
    stats_suggestion: CharacterStats,
    weight: f32,
}

impl Race {
    pub fn get_classes(&self) -> Arc<Vec<GameObjectRef>> {
        self.classes.clone()
    }

    pub fn get_description(&self) -> String {
        self.description.clone()
    }

    pub fn get_height(&self) -> f32 {
        self.height
    }

    pub fn get_stats(&self) -> CharacterStats {
        self.stats.clone()
    }

    pub fn get_stats_suggestion(&self) -> CharacterStats {
        self.stats_suggestion.clone()
    }

    pub fn get_weight(&self) -> f32 {
        self.weight
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Arc<dyn GameObject>, String> {
        match serde_json::from_str::<RaceDto>(json) {
            Ok(race_dto) => Ok(Arc::new(Self {
                id,
                adjective: race_dto.adjective,
                classes: Arc::new(race_dto.classes),
                description: race_dto.description,
                height: race_dto.height,
                name: race_dto.name,
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
    fn get_adjective(&self) -> &str {
        &self.adjective
    }

    fn get_id(&self) -> GameObjectId {
        self.id
    }

    fn get_object_type(&self) -> GameObjectType {
        GameObjectType::Race
    }

    fn get_name(&self) -> &str {
        &self.name
    }

    fn to_race(&self) -> Option<Self> {
        Some(self.clone())
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct RaceDto {
    adjective: String,
    classes: Vec<GameObjectRef>,
    description: String,
    height: f32,
    name: String,
    startingRoom: GameObjectRef,
    stats: CharacterStats,
    statsSuggestion: CharacterStats,
    weight: f32,
}
