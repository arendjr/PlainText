use std::borrow::Borrow;
use std::convert::AsRef;
use std::ops::Deref;
use std::sync::Arc;

use super::GameObject;

#[derive(Clone)]
pub struct SharedGameObject {
    object: Arc<dyn GameObject>,
}

impl SharedGameObject {
    pub fn new<T: GameObject>(object: T) -> Self
    where
        T: 'static,
    {
        Self {
            object: Arc::new(object),
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

type FromShared<T: GameObject + 'static> = dyn Fn(&SharedGameObject) -> Option<&T>;

pub struct SharedObject<T: GameObject + 'static> {
    cast: Box<FromShared<T>>,
    object: SharedGameObject,
}

impl<T: GameObject + 'static> SharedObject<T> {
    pub fn new<F>(object: SharedGameObject, cast: F) -> Option<Self>
    where
        F: Fn(&SharedGameObject) -> Option<&T> + 'static,
    {
        match cast(&object) {
            Some(_) => Some(Self {
                cast: Box::new(cast),
                object,
            }),
            None => None,
        }
    }
}

impl<T: GameObject + 'static> PartialEq for SharedObject<T> {
    fn eq(&self, other: &Self) -> bool {
        self.object.id() == other.object.id()
    }
}

impl<T: GameObject + 'static> Deref for SharedObject<T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        (self.cast)(&self.object).unwrap()
    }
}
