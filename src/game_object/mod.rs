mod character;
mod game_object_ref;
mod game_object_util;

pub use character::{Character, Gender};
pub use game_object_ref::{GameObjectId, GameObjectRef, GameObjectType};
pub use game_object_util::{ref_difference, ref_union};

use core::panic;

use crate::objects;

pub trait GameObject {
    fn id(&self) -> GameObjectId;
    fn needs_sync(&self) -> bool;
    fn object_type(&self) -> GameObjectType;
    fn set_needs_sync(&mut self, needs_sync: bool);

    fn adjective(&self) -> &str {
        ""
    }

    fn as_character(&self) -> Option<&dyn Character> {
        None
    }

    fn as_character_mut(&mut self) -> Option<&mut dyn Character> {
        None
    }

    fn as_class(&self) -> Option<&objects::Class> {
        None
    }

    fn as_class_mut(&mut self) -> Option<&mut objects::Class> {
        None
    }

    fn as_item(&self) -> Option<&objects::Item> {
        None
    }

    fn as_item_mut(&mut self) -> Option<&mut objects::Item> {
        None
    }

    fn as_npc(&self) -> Option<&objects::Npc> {
        None
    }

    fn as_npc_mut(&mut self) -> Option<&mut objects::Npc> {
        None
    }

    fn as_object(&self) -> Option<&dyn GameObject> {
        None
    }

    fn as_object_mut(&mut self) -> Option<&mut dyn GameObject> {
        None
    }

    fn as_player(&self) -> Option<&objects::Player> {
        None
    }

    fn as_player_mut(&mut self) -> Option<&mut objects::Player> {
        None
    }

    fn as_portal(&self) -> Option<&objects::Portal> {
        None
    }

    fn as_portal_mut(&mut self) -> Option<&mut objects::Portal> {
        None
    }

    fn as_race(&self) -> Option<&objects::Race> {
        None
    }

    fn as_race_mut(&mut self) -> Option<&mut objects::Race> {
        None
    }

    fn as_realm(&self) -> Option<&objects::Realm> {
        None
    }

    fn as_room(&self) -> Option<&objects::Room> {
        None
    }

    fn as_room_mut(&mut self) -> Option<&mut objects::Room> {
        None
    }

    fn dehydrate(&self) -> serde_json::Value;

    fn description(&self) -> &str;
    fn set_description(&mut self, description: String);

    fn indefinite_article(&self) -> &str {
        ""
    }

    fn indefinite_name(&self) -> String {
        if self.indefinite_article().is_empty() {
            self.name().to_owned()
        } else {
            format!("{} {}", self.indefinite_article(), self.name())
        }
    }

    fn name(&self) -> &str;
    fn set_name(&mut self, name: String);

    /// Returns the name of the object or a more fuzzy description, depending on the strength
    /// (clarity) with which the object is being observed.
    ///
    /// The clarity with which the object is being observed is indicated through the strength,
    /// where 1.0 represents a full clear view, while 0.0 means the object has become invisible.
    fn name_at_strength(&self, strength: f32) -> String {
        if strength >= 1.0 {
            self.name().to_owned()
        } else {
            "something".to_owned()
        }
    }

    fn object_ref(&self) -> GameObjectRef {
        GameObjectRef(self.object_type(), self.id())
    }

    fn plural_form(&self) -> &str {
        self.name()
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String>;
}

pub fn hydrate(object_ref: GameObjectRef, content: &str) -> Result<Box<dyn GameObject>, String> {
    let hydrate = match object_ref.object_type() {
        GameObjectType::Class => objects::Class::hydrate,
        GameObjectType::Item => objects::Item::hydrate,
        GameObjectType::Npc => objects::Npc::hydrate,
        GameObjectType::Player => objects::Player::hydrate,
        GameObjectType::Portal => objects::Portal::hydrate,
        GameObjectType::Race => objects::Race::hydrate,
        GameObjectType::Realm => panic!("Only one realm can be loaded"),
        GameObjectType::Room => objects::Room::hydrate,
    };

    hydrate(object_ref.id(), content)
}

#[derive(PartialEq)]
pub enum GameObjectPersistence {
    Sync,
    DontSync,
}

#[macro_export]
macro_rules! game_object_copy_prop {
    ($name:ident, $setter:ident, $type:ty) => {
        game_object_copy_prop!(, $name, $setter, $type, crate::game_object::GameObjectPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident, $type:ty) => {
        game_object_copy_prop!($vis, $name, $setter, $type, crate::game_object::GameObjectPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident, $type:ty, $persistence:expr) => {
        $vis fn $name(&self) -> $type {
            self.$name
        }

        $vis fn $setter(&mut self, $name: $type) {
            self.$name = $name;

            if ($persistence == crate::game_object::GameObjectPersistence::Sync) {
                self.set_needs_sync(true);
            }
        }
    };
}

#[macro_export]
macro_rules! game_object_ref_prop {
    ($name:ident, $setter:ident, $type:ty) => {
        game_object_ref_prop!(, $name, $setter, $type, crate::game_object::GameObjectPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident, $type:ty) => {
        game_object_ref_prop!($vis, $name, $setter, $type, crate::game_object::GameObjectPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident, $type:ty, $persistence:expr) => {
        $vis fn $name(&self) -> &$type {
            &self.$name
        }

        $vis fn $setter(&mut self, $name: $type) {
            self.$name = $name;

            if ($persistence == crate::game_object::GameObjectPersistence::Sync) {
                self.set_needs_sync(true);
            }
        }
    };
}

#[macro_export]
macro_rules! game_object_string_prop {
    ($name:ident, $setter:ident) => {
        game_object_string_prop!(, $name, $setter, crate::game_object::GameObjectPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident) => {
        game_object_string_prop!($vis, $name, $setter, crate::game_object::GameObjectPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident, $persistence:expr) => {
        $vis fn $name(&self) -> &str {
            &self.$name
        }

        $vis fn $setter(&mut self, $name: String) {
            self.$name = $name;

            if ($persistence == crate::game_object::GameObjectPersistence::Sync) {
                self.set_needs_sync(true);
            }
        }
    };
}
