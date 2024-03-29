use serde::de::{self, Deserialize, Deserializer, SeqAccess, Visitor};
use serde::ser::{Serialize, SerializeTupleStruct, Serializer};
use std::borrow::Borrow;
use std::{fmt, ops};

pub const NUM_STATS: usize = 6;

pub enum CharacterStat {
    Strength = 0,
    Dexterity,
    Vitality,
    Endurance,
    Intelligence,
    Faith,
}

#[derive(Clone, Debug, Default, PartialEq)]
pub struct CharacterStats {
    stats: [i16; NUM_STATS],
}

impl CharacterStats {
    pub fn from_stats(
        strength: i16,
        dexterity: i16,
        vitality: i16,
        endurance: i16,
        intelligence: i16,
        faith: i16,
    ) -> Self {
        Self {
            stats: [
                strength,
                dexterity,
                vitality,
                endurance,
                intelligence,
                faith,
            ],
        }
    }

    pub fn from_str(stats_str: &str) -> Result<Self, String> {
        if !stats_str.starts_with('[') || !stats_str.ends_with(']') {
            return Err("Invalid stats -- use square brackets to denote stats".to_owned());
        }

        let parts: Vec<&str> = stats_str[1..stats_str.len() - 1].split(',').collect();
        if parts.len() != NUM_STATS {
            return Err(format!(
                "Invalid stats -- must contain {:?} comma-separated stats",
                NUM_STATS
            ));
        }

        Ok(Self::from_stats(
            parts[0]
                .parse()
                .map_err(|err| format!("Could not parse STR: {:?}", err))?,
            parts[1]
                .parse()
                .map_err(|err| format!("Could not parse DEX: {:?}", err))?,
            parts[2]
                .parse()
                .map_err(|err| format!("Could not parse VIT: {:?}", err))?,
            parts[3]
                .parse()
                .map_err(|err| format!("Could not parse END: {:?}", err))?,
            parts[4]
                .parse()
                .map_err(|err| format!("Could not parse INT: {:?}", err))?,
            parts[5]
                .parse()
                .map_err(|err| format!("Could not parse FTH: {:?}", err))?,
        ))
    }

    pub fn get(&self, stat: CharacterStat) -> i16 {
        self.stats[stat as usize]
    }

    pub fn inc(&mut self, stat: CharacterStat) {
        self.stats[stat as usize] += 1;
    }

    pub fn max_hp(&self) -> i16 {
        2 * self.get(CharacterStat::Vitality)
    }

    pub fn max_mp(&self) -> i16 {
        self.get(CharacterStat::Intelligence)
    }

    pub fn new() -> Self {
        Self {
            stats: [0, 0, 0, 0, 0, 0],
        }
    }

    pub fn set(&mut self, stat: CharacterStat, value: i16) {
        self.stats[stat as usize] = value;
    }

    pub fn total(&self) -> i16 {
        (0..NUM_STATS).fold(0, |total, i| total + self.stats[i])
    }
}

impl<T> ops::Add<T> for CharacterStats
where
    T: Borrow<Self>,
{
    type Output = Self;

    fn add(self, rhs: T) -> Self::Output {
        Self {
            stats: [
                self.stats[0] + rhs.borrow().stats[0],
                self.stats[1] + rhs.borrow().stats[1],
                self.stats[2] + rhs.borrow().stats[2],
                self.stats[3] + rhs.borrow().stats[3],
                self.stats[4] + rhs.borrow().stats[4],
                self.stats[5] + rhs.borrow().stats[5],
            ],
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
        write!(f, "CharacterStats({:?})", self.stats)
    }
}

impl Serialize for CharacterStats {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let mut state = serializer.serialize_tuple_struct("CharacterStats", NUM_STATS)?;
        for i in 0..NUM_STATS {
            state.serialize_field(&self.stats[i])?;
        }
        state.end()
    }
}

struct CharacterStatsVisitor;

impl<'de> Visitor<'de> for CharacterStatsVisitor {
    type Value = CharacterStats;

    fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "an array with six integers")
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
        Ok(CharacterStats::from_stats(
            strength,
            dexterity,
            vitality,
            endurance,
            intelligence,
            faith,
        ))
    }
}
