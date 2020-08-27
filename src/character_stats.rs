use serde::de::{self, Deserialize, Deserializer, SeqAccess, Visitor};
use serde::ser::{Serialize, SerializeTupleStruct, Serializer};
use std::fmt;

pub const STRENGTH: usize = 0;
pub const DEXTERITY: usize = 1;
pub const VITALITY: usize = 2;
pub const ENDURANCE: usize = 3;
pub const INTELLIGENCE: usize = 4;
pub const FAITH: usize = 5;

#[derive(Clone, Debug)]
pub struct CharacterStats(i16, i16, i16, i16, i16, i16);

impl CharacterStats {
    pub fn new() -> Self {
        Self {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
        }
    }
}

impl<'de> Deserialize<'de> for CharacterStats {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        deserializer.deserialize_seq(CharacterStatsVisitor)
    }
}

impl fmt::Display for CharacterStats {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "CharacterStats({}, {}, {}, {}, {}, {})",
            self.0, self.1, self.2, self.3, self.4, self.5
        )
    }
}

impl Serialize for CharacterStats {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let mut state = serializer.serialize_tuple_struct("CharacterStats", 6)?;
        state.serialize_field(&self.0)?;
        state.serialize_field(&self.1)?;
        state.serialize_field(&self.2)?;
        state.serialize_field(&self.3)?;
        state.serialize_field(&self.4)?;
        state.serialize_field(&self.5)?;
        state.end()
    }
}

struct CharacterStatsVisitor;

impl<'de> Visitor<'de> for CharacterStatsVisitor {
    type Value = CharacterStats;

    fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        formatter.write_str("an array with six integers")
    }

    fn visit_seq<A>(self, mut seq: A) -> Result<Self::Value, A::Error>
    where
        A: SeqAccess<'de>,
    {
        let strength = seq
            .next_element()?
            .ok_or_else(|| de::Error::invalid_length(0, &self))?;
        let dexterity = seq
            .next_element()?
            .ok_or_else(|| de::Error::invalid_length(1, &self))?;
        let vitality = seq
            .next_element()?
            .ok_or_else(|| de::Error::invalid_length(2, &self))?;
        let endurance = seq
            .next_element()?
            .ok_or_else(|| de::Error::invalid_length(3, &self))?;
        let intelligence = seq
            .next_element()?
            .ok_or_else(|| de::Error::invalid_length(4, &self))?;
        let faith = seq
            .next_element()?
            .ok_or_else(|| de::Error::invalid_length(5, &self))?;
        Ok(CharacterStats(
            strength,
            dexterity,
            vitality,
            endurance,
            intelligence,
            faith,
        ))
    }
}
