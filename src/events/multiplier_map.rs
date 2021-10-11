use serde::de::{Deserialize, Deserializer, MapAccess, Visitor};
use serde::ser::{Serialize, SerializeMap, Serializer};
use std::fmt;

use super::event_types::EventType;

const DEFAULT_MULTIPLIERS: [f32; EventType::NUM_EVENT_TYPES] = [1.0; EventType::NUM_EVENT_TYPES];

#[derive(Clone, Debug, PartialEq)]
pub struct MultiplierMap {
    multipliers: [f32; EventType::NUM_EVENT_TYPES],
}

impl MultiplierMap {
    pub fn get(&self, event_type: EventType) -> f32 {
        self.multipliers[event_type as usize]
    }

    pub fn is_default(&self) -> bool {
        self.multipliers == DEFAULT_MULTIPLIERS
    }

    pub fn new() -> Self {
        Self {
            multipliers: DEFAULT_MULTIPLIERS,
        }
    }

    pub fn set(&mut self, event_type: EventType, value: f32) {
        self.multipliers[event_type as usize] = value;
    }
}

impl Default for MultiplierMap {
    fn default() -> Self {
        MultiplierMap::new()
    }
}

impl<'de> Deserialize<'de> for MultiplierMap {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        deserializer.deserialize_map(MultiplierMapVisitor)
    }
}

impl fmt::Display for MultiplierMap {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "MultiplierMap({:?})", self.multipliers)
    }
}

impl Serialize for MultiplierMap {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let mut map = serializer.serialize_map(None)?;
        for event_type in &EventType::ALL {
            let value = self.multipliers[*event_type as usize];
            if value != 0.0 {
                map.serialize_entry(event_type.serialize(), &value)?;
            }
        }
        map.end()
    }
}

struct MultiplierMapVisitor;

impl<'de> Visitor<'de> for MultiplierMapVisitor {
    type Value = MultiplierMap;

    fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "an object with floating point values")
    }

    fn visit_map<A>(self, mut access: A) -> Result<Self::Value, A::Error>
    where
        A: MapAccess<'de>,
    {
        let mut map = MultiplierMap::new();
        while let Ok(Some((key, value))) = access.next_entry() {
            if let Some(event_type) = EventType::deserialize(key) {
                map.set(event_type, value);
            } else {
                println!("Dropped multiplier for unknown event type \"{}\"", key);
            }
        }
        Ok(map)
    }
}
