use super::EntityRef;
use serde::{Deserialize, Serialize};
use std::{cell::RefCell, rc::Rc};

mod actor;
mod guard;
mod housewife;

pub use actor::Actor;
pub use guard::Guard;
pub use housewife::Housewife;

#[derive(Clone, Copy, Debug, Deserialize, Serialize)]
#[serde(rename_all = "snake_case")]
pub enum Behavior {
    Guard,
    Housewife,
}

impl Behavior {
    pub fn new_actor(&self, entity_ref: EntityRef) -> Rc<RefCell<dyn Actor>> {
        match self {
            Self::Guard => Rc::new(RefCell::new(Guard::new(entity_ref))),
            Self::Housewife => Rc::new(RefCell::new(Housewife::new(entity_ref))),
        }
    }
}
