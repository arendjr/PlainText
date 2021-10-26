use crate::{
    character_stats::CharacterStats,
    entity::{Entity, EntityId, EntityRef, EntityType},
    entity_ref_prop, entity_string_prop,
};
use serde::{Deserialize, Serialize};

#[derive(Clone, Debug, Deserialize, PartialEq, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Class {
    #[serde(skip)]
    id: EntityId,
    description: String,
    name: String,
    #[serde(skip)]
    needs_sync: bool,
    stats: CharacterStats,
    #[serde(default)]
    stats_suggestion: CharacterStats,
}

impl Class {
    entity_ref_prop!(pub, stats, set_stats, CharacterStats);
    entity_ref_prop!(pub, stats_suggestion, set_stats_suggestion, CharacterStats);

    pub fn hydrate(id: EntityId, json: &str) -> Result<Box<dyn Entity>, String> {
        let mut class = serde_json::from_str::<Class>(json)
            .map_err(|error| format!("parse error: {}", error))?;
        class.id = id;
        Ok(Box::new(class))
    }
}

impl Entity for Class {
    entity_string_prop!(name, set_name);
    entity_string_prop!(description, set_description);

    fn as_class(&self) -> Option<&Self> {
        Some(self)
    }

    fn as_class_mut(&mut self) -> Option<&mut Self> {
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
        EntityRef::new(EntityType::Class, self.id)
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
            "description" => {
                self.set_description(value.to_owned());
                Ok(())
            }
            "name" => {
                self.set_name(value.to_owned());
                Ok(())
            }
            "stats" => {
                self.set_stats(CharacterStats::from_str(value)?);
                Ok(())
            }
            "statsSuggestion" => {
                self.set_stats_suggestion(CharacterStats::from_str(value)?);
                Ok(())
            }
            _ => Err(format!("No property named \"{}\"", prop_name)),
        }
    }

    fn to_json_value(&self) -> serde_json::Result<serde_json::Value> {
        serde_json::to_value(self)
    }
}
