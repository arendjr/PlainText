use evmap;
use std::hash::{Hash, Hasher};
use std::sync::Arc;

use super::{GameObject, GameObjectRef};

impl Eq for dyn GameObject {}

impl Hash for dyn GameObject {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.get_id().hash(state);
        self.get_object_type().hash(state);
    }
}

impl PartialEq for dyn GameObject {
    fn eq(&self, other: &Self) -> bool {
        self.get_id() == other.get_id()
    }
}

pub fn new() -> (
    evmap::ReadHandle<GameObjectRef, Arc<dyn GameObject>>,
    evmap::WriteHandle<GameObjectRef, Arc<dyn GameObject>>,
) {
    evmap::new()
}
