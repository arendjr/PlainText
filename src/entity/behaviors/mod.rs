use super::EntityRef;
use serde::{Deserialize, Serialize};
use std::{cell::RefCell, rc::Rc};

mod actor;
mod guard;

pub use actor::Actor;
pub use guard::Guard;

#[derive(Clone, Copy, Debug, Deserialize, Serialize)]
#[serde(rename_all = "snake_case")]
pub enum Behavior {
    Guard,
}

impl Behavior {
    pub fn new_actor<'a>(&self, entity_ref: EntityRef) -> Rc<RefCell<dyn Actor>> {
        match self {
            Self::Guard => Rc::new(RefCell::new(Guard::new(entity_ref))),
        }
    }
}
