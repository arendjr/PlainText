use crate::{entity::EntityRef, entity_copy_prop};
use serde::{Deserialize, Serialize};
use std::time::Duration;

/// Component that tracks state of entities that respawn on death/consumption/use/etc..
#[derive(Clone, Debug, Deserialize, PartialEq, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Respawnable {
    max_respawn_time: Duration,

    min_respawn_time: Duration,

    #[serde(skip)]
    needs_sync: bool,

    spawning_room: EntityRef,
}

impl Respawnable {
    entity_copy_prop!(pub, max_respawn_time, set_max_respawn_time, Duration);
    entity_copy_prop!(pub, min_respawn_time, set_min_respawn_time, Duration);

    pub fn needs_sync(&self) -> bool {
        self.needs_sync
    }

    pub fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;
    }

    entity_copy_prop!(pub, spawning_room, set_spawning_room, EntityRef);

    pub fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "maxRespawnTime" => self.set_max_respawn_time(
                value
                    .parse::<u64>()
                    .map(Duration::from_millis)
                    .map_err(|error| format!("Invalid duration: {}", error))?,
            ),
            "minRespawnTime" => self.set_min_respawn_time(
                value
                    .parse::<u64>()
                    .map(Duration::from_millis)
                    .map_err(|error| format!("Invalid duration: {}", error))?,
            ),
            "spawningRoom" => self.set_spawning_room(EntityRef::from_str(value)?),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }
}
