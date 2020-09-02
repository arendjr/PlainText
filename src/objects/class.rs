use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;
use std::sync::Arc;

use crate::character_stats::CharacterStats;
use crate::game_object::{GameObject, GameObjectId, GameObjectType};

#[derive(Clone, Debug, PartialEq)]
pub struct Class {
    id: GameObjectId,
    description: String,
    name: String,
    stats: CharacterStats,
    stats_suggestion: CharacterStats,
}

impl Class {
    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Arc<dyn GameObject>, String> {
        match serde_json::from_str::<ClassDto>(json) {
            Ok(class_dto) => Ok(Arc::new(Self {
                id,
                description: class_dto.description,
                name: class_dto.name,
                stats: class_dto.stats,
                stats_suggestion: match class_dto.statsSuggestion {
                    Some(stats_suggestion) => stats_suggestion,
                    None => CharacterStats::new(),
                },
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn stats(&self) -> CharacterStats {
        self.stats.clone()
    }

    pub fn stats_suggestion(&self) -> CharacterStats {
        self.stats_suggestion.clone()
    }
}

impl fmt::Display for Class {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Class({}, {})", self.id, self.name)
    }
}

impl GameObject for Class {
    fn description(&self) -> &str {
        &self.description
    }

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Class
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn to_class(&self) -> Option<Self> {
        Some(self.clone())
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct ClassDto {
    description: String,
    name: String,
    stats: CharacterStats,
    statsSuggestion: Option<CharacterStats>,
}
