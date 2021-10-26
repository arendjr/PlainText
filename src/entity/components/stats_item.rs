use crate::character_stats::CharacterStats;
use crate::entity_ref_prop;
use serde::{Deserialize, Serialize};

/// Component for any item or character that has associated stats.
#[derive(Clone, Debug, Deserialize, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct StatsItem {
    #[serde(skip)]
    needs_sync: bool,

    stats: CharacterStats,
}

impl StatsItem {
    pub fn from_stats(stats: CharacterStats) -> Self {
        Self {
            needs_sync: false,
            stats,
        }
    }

    pub fn needs_sync(&self) -> bool {
        self.needs_sync
    }

    pub fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;
    }

    entity_ref_prop!(pub, stats, set_stats, CharacterStats);

    pub fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "stats" => self.set_stats(CharacterStats::from_str(value)?),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }
}
