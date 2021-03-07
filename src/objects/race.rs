use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::character_stats::CharacterStats;
use crate::game_object::{GameObject, GameObjectId, GameObjectRef, GameObjectType};

#[derive(Clone, Debug, PartialEq)]
pub struct Race {
    id: GameObjectId,
    adjective: String,
    classes: Vec<GameObjectRef>,
    description: String,
    height: f32,
    name: String,
    needs_sync: bool,
    starting_room: GameObjectRef,
    stats: CharacterStats,
    stats_suggestion: CharacterStats,
    weight: f32,
}

impl Race {
    game_object_string_prop!(pub, adjective, set_adjective);
    game_object_ref_prop!(pub, classes, set_classes, Vec<GameObjectRef>);
    game_object_copy_prop!(pub, height, set_height, f32);
    game_object_copy_prop!(pub, starting_room, set_starting_room, GameObjectRef);
    game_object_ref_prop!(pub, stats, set_stats, CharacterStats);
    game_object_ref_prop!(pub, stats_suggestion, set_stats_suggestion, CharacterStats);
    game_object_copy_prop!(pub, weight, set_weight, f32);

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Box<dyn GameObject>, String> {
        match serde_json::from_str::<RaceDto>(json) {
            Ok(race_dto) => Ok(Box::new(Self {
                id,
                adjective: race_dto.adjective,
                classes: race_dto.classes,
                description: race_dto.description,
                height: race_dto.height,
                name: race_dto.name,
                needs_sync: false,
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
    game_object_string_prop!(,name, set_name);
    game_object_string_prop!(,description, set_description);

    fn adjective(&self) -> &str {
        &self.adjective
    }

    fn as_race(&self) -> Option<&Self> {
        Some(self)
    }

    fn as_race_mut(&mut self) -> Option<&mut Self> {
        Some(self)
    }

    fn as_object(&self) -> Option<&dyn GameObject> {
        Some(self)
    }

    fn as_object_mut(&mut self) -> Option<&mut dyn GameObject> {
        Some(self)
    }

    fn dehydrate(&self) -> serde_json::Value {
        serde_json::to_value(RaceDto {
            adjective: self.adjective.clone(),
            classes: self.classes.clone(),
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

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn needs_sync(&self) -> bool {
        self.needs_sync
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Race
    }

    fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "adjective" => Ok(self.set_adjective(value.to_owned())),
            "classes" => Ok(self.set_classes(GameObjectRef::vec_from_str(value)?)),
            "description" => Ok(self.set_description(value.to_owned())),
            "height" => Ok(self.set_height(value.parse().map_err(|error| format!("{:?}", error))?)),
            "name" => Ok(self.set_name(value.to_owned())),
            "startingRoom" => Ok(self.set_starting_room(GameObjectRef::from_str(value)?)),
            "stats" => Ok(self.set_stats(CharacterStats::from_str(value)?)),
            "statsSuggestion" => Ok(self.set_stats_suggestion(CharacterStats::from_str(value)?)),
            "weight" => Ok(self.set_weight(value.parse().map_err(|error| format!("{:?}", error))?)),
            _ => Err(format!("No property named \"{}\"", prop_name))?,
        }
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
