use crate::objects;

use super::{Character, GameObjectId, GameObjectRef, GameObjectType, SharedGameObject};

pub trait GameObject {
    fn id(&self) -> GameObjectId;
    fn object_type(&self) -> GameObjectType;

    fn adjective(&self) -> &str {
        ""
    }

    fn as_character(&self) -> Option<&dyn Character> {
        None
    }

    fn as_class(&self) -> Option<&objects::Class> {
        None
    }

    fn as_item(&self) -> Option<&objects::Item> {
        None
    }

    fn as_npc(&self) -> Option<&objects::Npc> {
        None
    }

    fn as_player(&self) -> Option<&objects::Player> {
        None
    }

    fn as_portal(&self) -> Option<&objects::Portal> {
        None
    }

    fn as_race(&self) -> Option<&objects::Race> {
        None
    }

    fn as_realm(&self) -> Option<&objects::Realm> {
        None
    }

    fn as_room(&self) -> Option<&objects::Room> {
        None
    }

    fn dehydrate(&self) -> serde_json::Value;

    fn description(&self) -> &str;
    fn set_description(
        &self,
        realm: crate::objects::Realm,
        description: String,
    ) -> crate::objects::Realm;

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
    fn set_name(&self, realm: crate::objects::Realm, name: String) -> crate::objects::Realm;

    fn object_ref(&self) -> GameObjectRef {
        GameObjectRef(self.object_type(), self.id())
    }

    fn plural_form(&self) -> String {
        self.name().to_owned()
    }

    fn set_property(
        &self,
        realm: objects::Realm,
        prop_name: &str,
        value: &str,
    ) -> Result<objects::Realm, String>;
}

pub fn hydrate(object_ref: GameObjectRef, content: &str) -> Result<SharedGameObject, String> {
    let hydrate = match object_ref.object_type() {
        GameObjectType::Class => objects::Class::hydrate,
        GameObjectType::Item => objects::Item::hydrate,
        GameObjectType::Npc => objects::Npc::hydrate,
        GameObjectType::Player => objects::Player::hydrate,
        GameObjectType::Portal => objects::Portal::hydrate,
        GameObjectType::Race => objects::Race::hydrate,
        GameObjectType::Realm => objects::Realm::hydrate,
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

        $vis fn $setter(&self, realm: crate::objects::Realm, $name: $type) -> crate::objects::Realm {
            realm.set_shared_object(
                self.object_ref(),
                SharedGameObject::new(Self { $name, ..self.clone() }),
                $persistence
            )
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

        $vis fn $setter(&self, realm: crate::objects::Realm, $name: $type) -> crate::objects::Realm {
            realm.set_shared_object(
                self.object_ref(),
                SharedGameObject::new(Self { $name, ..self.clone() }),
                $persistence
            )
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

        $vis fn $setter(&self, realm: crate::objects::Realm, $name: String) -> crate::objects::Realm {
            realm.set_shared_object(
                self.object_ref(),
                SharedGameObject::new(Self { $name, ..self.clone() }),
                $persistence
            )
        }
    };
}
