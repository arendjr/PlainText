use core::panic;

mod behaviors;
mod components;
mod entities;
mod entity_ref;
mod entity_util;

pub use behaviors::*;
pub use components::*;
pub use entities::*;
pub use entity_ref::{EntityId, EntityRef, EntityType};
pub use entity_util::{ref_difference, ref_union};

pub trait Entity {
    fn id(&self) -> EntityId;
    fn needs_sync(&self) -> bool;
    fn set_needs_sync(&mut self, needs_sync: bool);

    fn adjective(&self) -> &str {
        ""
    }

    fn as_actor_state(&self) -> Option<&ActorState> {
        None
    }

    fn as_actor_state_mut(&mut self) -> Option<&mut ActorState> {
        None
    }

    fn as_character(&self) -> Option<&Character> {
        None
    }

    fn as_character_mut(&mut self) -> Option<&mut Character> {
        None
    }

    fn as_class(&self) -> Option<&entities::Class> {
        None
    }

    fn as_class_mut(&mut self) -> Option<&mut entities::Class> {
        None
    }

    fn as_group(&self) -> Option<&entities::Group> {
        None
    }

    fn as_group_mut(&mut self) -> Option<&mut entities::Group> {
        None
    }

    fn as_item(&self) -> Option<&entities::Item> {
        None
    }

    fn as_item_mut(&mut self) -> Option<&mut entities::Item> {
        None
    }

    fn as_npc(&self) -> Option<&entities::Npc> {
        None
    }

    fn as_npc_mut(&mut self) -> Option<&mut entities::Npc> {
        None
    }

    fn as_openable(&self) -> Option<&Openable> {
        None
    }

    fn as_openable_mut(&mut self) -> Option<&mut Openable> {
        None
    }

    fn as_player(&self) -> Option<&entities::Player> {
        None
    }

    fn as_player_mut(&mut self) -> Option<&mut entities::Player> {
        None
    }

    fn as_portal(&self) -> Option<&entities::Portal> {
        None
    }

    fn as_portal_mut(&mut self) -> Option<&mut entities::Portal> {
        None
    }

    fn as_race(&self) -> Option<&entities::Race> {
        None
    }

    fn as_race_mut(&mut self) -> Option<&mut entities::Race> {
        None
    }

    fn as_realm(&self) -> Option<&entities::Realm> {
        None
    }

    fn as_respawnable(&self) -> Option<&Respawnable> {
        None
    }

    fn as_respawnable_mut(&mut self) -> Option<&mut Respawnable> {
        None
    }

    fn as_room(&self) -> Option<&entities::Room> {
        None
    }

    fn as_room_mut(&mut self) -> Option<&mut entities::Room> {
        None
    }

    fn as_stats_item(&self) -> Option<&StatsItem> {
        None
    }

    fn as_stats_item_mut(&mut self) -> Option<&mut StatsItem> {
        None
    }

    fn dehydrate(&self) -> String;

    fn description(&self) -> &str;
    fn set_description(&mut self, description: String);

    fn entity_ref(&self) -> EntityRef;

    fn entity_type(&self) -> EntityType {
        self.entity_ref().entity_type()
    }

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

    fn plural_form(&self) -> &str {
        self.name()
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String>;

    fn to_json_value(&self) -> serde_json::Result<serde_json::Value>;
}

pub fn hydrate(entity_ref: EntityRef, content: &str) -> Result<Box<dyn Entity>, String> {
    let hydrate = match entity_ref.entity_type() {
        EntityType::Class => entities::Class::hydrate,
        EntityType::Group => panic!("Groups cannot be hydrated"),
        EntityType::Item => entities::Item::hydrate,
        EntityType::Npc => entities::Npc::hydrate,
        EntityType::Player => entities::Player::hydrate,
        EntityType::Portal => entities::Portal::hydrate,
        EntityType::Race => entities::Race::hydrate,
        EntityType::Realm => panic!("Only one realm can be loaded"),
        EntityType::Room => entities::Room::hydrate,
    };

    hydrate(entity_ref.id(), content)
}

#[derive(PartialEq)]
pub enum EntityPersistence {
    Sync,
    DontSync,
}

#[macro_export]
macro_rules! entity_copy_prop {
    ($name:ident, $setter:ident, $type:ty) => {
        entity_copy_prop!(, $name, $setter, $type, crate::entity::EntityPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident, $type:ty) => {
        entity_copy_prop!($vis, $name, $setter, $type, crate::entity::EntityPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident, $type:ty, $persistence:expr) => {
        $vis fn $name(&self) -> $type {
            self.$name
        }

        $vis fn $setter(&mut self, $name: $type) {
            self.$name = $name;

            if ($persistence == crate::entity::EntityPersistence::Sync) {
                self.set_needs_sync(true);
            }
        }
    };
}

#[macro_export]
macro_rules! entity_ref_prop {
    ($name:ident, $setter:ident, $type:ty) => {
        entity_ref_prop!(, $name, $setter, $type, crate::entity::EntityPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident, $type:ty) => {
        entity_ref_prop!($vis, $name, $setter, $type, crate::entity::EntityPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident, $type:ty, $persistence:expr) => {
        $vis fn $name(&self) -> &$type {
            &self.$name
        }

        $vis fn $setter(&mut self, $name: $type) {
            self.$name = $name;

            if ($persistence == crate::entity::EntityPersistence::Sync) {
                self.set_needs_sync(true);
            }
        }
    };
}

#[macro_export]
macro_rules! entity_string_prop {
    ($name:ident, $setter:ident) => {
        entity_string_prop!(, $name, $setter, crate::entity::EntityPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident) => {
        entity_string_prop!($vis, $name, $setter, crate::entity::EntityPersistence::Sync);
    };

    ($vis:vis, $name:ident, $setter:ident, $persistence:expr) => {
        $vis fn $name(&self) -> &str {
            &self.$name
        }

        $vis fn $setter(&mut self, $name: String) {
            self.$name = $name;

            if ($persistence == crate::entity::EntityPersistence::Sync) {
                self.set_needs_sync(true);
            }
        }
    };
}
