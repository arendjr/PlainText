use std::borrow::Borrow;
use std::convert::AsRef;
use std::ops::Deref;
use std::rc::Rc;

use super::GameObject;

#[derive(Clone)]
pub struct SharedGameObject {
    object: Rc<dyn GameObject>,
}

impl SharedGameObject {
    pub fn new<T: GameObject>(object: T) -> Self
    where
        T: 'static,
    {
        Self {
            object: Rc::new(object),
        }
    }
}

impl AsRef<dyn GameObject> for SharedGameObject {
    fn as_ref(&self) -> &(dyn GameObject + 'static) {
        self.object.borrow()
    }
}

impl Deref for SharedGameObject {
    type Target = dyn GameObject;

    fn deref(&self) -> &Self::Target {
        self.object.deref()
    }
}
