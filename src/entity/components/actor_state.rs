use crate::entity::EntityRef;
use futures::future::AbortHandle;
use std::collections::{BTreeMap, BTreeSet};

#[derive(Clone, Debug, PartialEq)]
enum MiscValue {
    Bool(bool),
}

/// Component that tracks state of entities that can be opened and closed.
#[derive(Clone, Debug, Default)]
pub struct ActorState {
    pub abort_handle: Option<AbortHandle>,
    pub enemies: BTreeSet<EntityRef>,
    misc: BTreeMap<String, MiscValue>,
}

impl ActorState {
    pub fn clear_misc(&mut self) {
        self.misc.clear();
    }

    pub fn get_misc_bool(&self, prop_name: &str) -> bool {
        self.misc
            .get(prop_name)
            .map(|value| match value {
                MiscValue::Bool(value) => *value,
                _ => false,
            })
            .unwrap_or(false)
    }

    pub fn set_misc_bool(&mut self, prop_name: &str, value: bool) {
        self.misc
            .insert(prop_name.to_owned(), MiscValue::Bool(value));
    }

    pub fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "enemies" => self.enemies = EntityRef::vec_from_str(value)?.into_iter().collect(),
            other => {
                self.misc
                    .insert(other.to_owned(), MiscValue::Bool(value == "true"));
            }
        }
        Ok(())
    }
}
