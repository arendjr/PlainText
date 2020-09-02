use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;
use std::ops::Deref;
use std::sync::Arc;

use crate::character_stats::CharacterStats;
use crate::game_object::{
    GameObject, GameObjectId, GameObjectRef, GameObjectType, SharedGameObject,
};

#[derive(Clone, Debug, PartialEq)]
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
    pub fn classes(&self) -> Arc<Vec<GameObjectRef>> {
        self.classes.clone()
    }

    pub fn description(&self) -> String {
        self.description.clone()
    }

    pub fn height(&self) -> f32 {
        self.height
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<SharedGameObject, String> {
        match serde_json::from_str::<RaceDto>(json) {
            Ok(race_dto) => Ok(SharedGameObject::new(Self {
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

    pub fn starting_room(&self) -> GameObjectRef {
        self.starting_room
    }

    pub fn stats(&self) -> CharacterStats {
        self.stats.clone()
    }

    pub fn stats_suggestion(&self) -> CharacterStats {
        self.stats_suggestion.clone()
    }

    pub fn weight(&self) -> f32 {
        self.weight
    }
}

impl fmt::Display for Race {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Race({}, {})", self.id, self.name)
    }
}

impl GameObject for Race {
    fn adjective(&self) -> &str {
        &self.adjective
    }

    fn as_race(&self) -> Option<&Self> {
        Some(&self)
    }

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Race
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn serialize(&self) -> String {
        serde_json::to_string_pretty(&RaceDto {
            adjective: self.adjective.clone(),
            classes: self.classes.deref().clone(),
            description: self.description.clone(),
            height: self.height,
            name: self.name.clone(),
            startingRoom: self.starting_room,
            stats: self.stats.clone(),
            statsSuggestion: self.stats_suggestion.clone(),
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
