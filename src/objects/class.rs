use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::character_stats::CharacterStats;
use crate::game_object::{GameObject, GameObjectId, GameObjectType};

#[derive(Clone, Debug, PartialEq)]
pub struct Class {
    id: GameObjectId,
    description: String,
    name: String,
    needs_sync: bool,
    stats: CharacterStats,
    stats_suggestion: CharacterStats,
}

impl Class {
    game_object_ref_prop!(pub, stats, set_stats, CharacterStats);
    game_object_ref_prop!(pub, stats_suggestion, set_stats_suggestion, CharacterStats);

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Box<dyn GameObject>, String> {
        match serde_json::from_str::<ClassDto>(json) {
            Ok(class_dto) => Ok(Box::new(Self {
                id,
                description: class_dto.description,
                name: class_dto.name,
                needs_sync: false,
                stats: class_dto.stats,
                stats_suggestion: match class_dto.statsSuggestion {
                    Some(stats_suggestion) => stats_suggestion,
                    None => CharacterStats::new(),
                },
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }
}

impl fmt::Display for Class {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Class({}, {})", self.id, self.name)
    }
}

impl GameObject for Class {
    game_object_string_prop!(name, set_name);
    game_object_string_prop!(description, set_description);

    fn as_class(&self) -> Option<&Self> {
        Some(self)
    }

    fn as_class_mut(&mut self) -> Option<&mut Self> {
        Some(self)
    }

    fn as_object(&self) -> Option<&dyn GameObject> {
        Some(self)
    }

    fn as_object_mut(&mut self) -> Option<&mut dyn GameObject> {
        Some(self)
    }

    fn dehydrate(&self) -> serde_json::Value {
        serde_json::to_value(ClassDto {
            description: self.description.clone(),
            name: self.name.clone(),
            stats: self.stats.clone(),
            statsSuggestion: Some(self.stats_suggestion.clone()),
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
        GameObjectType::Class
    }

    fn needs_sync(&self) -> bool {
        self.needs_sync
    }

    fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "description" => Ok(self.set_description(value.to_owned())),
            "name" => Ok(self.set_name(value.to_owned())),
            "stats" => Ok(self.set_stats(CharacterStats::from_str(value)?)),
            "statsSuggestion" => Ok(self.set_stats_suggestion(CharacterStats::from_str(value)?)),
            _ => Err(format!("No property named \"{}\"", prop_name))?,
        }
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
