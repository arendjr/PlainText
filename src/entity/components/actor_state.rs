use crate::entity::EntityRef;
use std::collections::BTreeSet;

/// Component that tracks state of entities that can be opened and closed.
#[derive(Clone, Debug, Default, PartialEq)]
pub struct ActorState {
    pub enemies: BTreeSet<EntityRef>,
}

impl ActorState {
    pub fn new() -> Self {
        Self {
            enemies: BTreeSet::new(),
        }
    }

    pub fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "enemies" => self.enemies = EntityRef::vec_from_str(value)?.into_iter().collect(),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }
}
