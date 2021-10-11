use crate::character_stats::CharacterStats;
use crate::entity::{Entity, EntityId, EntityRef, EntityType};
use crate::{entity_copy_prop, entity_ref_prop, entity_string_prop};
use serde::{Deserialize, Serialize};

#[derive(Clone, Debug, Deserialize, PartialEq, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Race {
    #[serde(skip)]
    id: EntityId,
    adjective: String,
    classes: Vec<EntityRef>,
    description: String,
    height: f32,
    name: String,
    #[serde(skip)]
    needs_sync: bool,
    starting_room: EntityRef,
    stats: CharacterStats,
    stats_suggestion: CharacterStats,
    weight: f32,
}

impl Race {
    entity_string_prop!(pub, adjective, set_adjective);
    entity_ref_prop!(pub, classes, set_classes, Vec<EntityRef>);
    entity_copy_prop!(pub, height, set_height, f32);
    entity_copy_prop!(pub, starting_room, set_starting_room, EntityRef);
    entity_ref_prop!(pub, stats, set_stats, CharacterStats);
    entity_ref_prop!(pub, stats_suggestion, set_stats_suggestion, CharacterStats);
    entity_copy_prop!(pub, weight, set_weight, f32);

    pub fn hydrate(id: EntityId, json: &str) -> Result<Box<dyn Entity>, String> {
        let mut race = serde_json::from_str::<Race>(json)
            .map_err(|error| format!("parse error: {}", error))?;
        race.id = id;
        Ok(Box::new(race))
    }
}

impl Entity for Race {
    entity_string_prop!(,name, set_name);
    entity_string_prop!(,description, set_description);

    fn adjective(&self) -> &str {
        &self.adjective
    }

    fn as_race(&self) -> Option<&Self> {
        Some(self)
    }

    fn as_race_mut(&mut self) -> Option<&mut Self> {
        Some(self)
    }

    fn as_entity(&self) -> Option<&dyn Entity> {
        Some(self)
    }

    fn as_entity_mut(&mut self) -> Option<&mut dyn Entity> {
        Some(self)
    }

    fn dehydrate(&self) -> String {
        serde_json::to_string_pretty(self).unwrap_or_else(|error| {
            panic!(
                "Failed to serialize entity {:?}: {:?}",
                self.entity_ref(),
                error
            )
        })
    }

    fn entity_ref(&self) -> EntityRef {
        EntityRef::new(EntityType::Race, self.id)
    }

    fn id(&self) -> EntityId {
        self.id
    }

    fn needs_sync(&self) -> bool {
        self.needs_sync
    }

    fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "adjective" => self.set_adjective(value.to_owned()),
            "classes" => self.set_classes(EntityRef::vec_from_str(value)?),
            "description" => self.set_description(value.to_owned()),
            "height" => self.set_height(value.parse().map_err(|error| format!("{:?}", error))?),
            "name" => self.set_name(value.to_owned()),
            "startingRoom" => self.set_starting_room(EntityRef::from_str(value)?),
            "stats" => self.set_stats(CharacterStats::from_str(value)?),
            "statsSuggestion" => self.set_stats_suggestion(CharacterStats::from_str(value)?),
            "weight" => self.set_weight(value.parse().map_err(|error| format!("{:?}", error))?),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }

    fn to_json_value(&self) -> serde_json::Result<serde_json::Value> {
        serde_json::to_value(self)
    }
}
