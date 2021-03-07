#![allow(non_upper_case_globals)]

use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::game_object::{GameObject, GameObjectId, GameObjectType, SharedGameObject};
use crate::point3d::Point3D;

use super::Realm;

serializable_flags! {
    pub struct ItemFlags: u32 {
        const AttachedToCeiling        = 0b00000001;
        const AttachedToWall           = 0b00000010;
        const Hidden                   = 0b00000100;
        const Portable                 = 0b00001000;
        const AlwaysUseDefiniteArticle = 0b00010000;
        const ImpliedPlural            = 0b00100000;
    }
}

#[derive(Clone, Debug)]
pub struct Item {
    id: GameObjectId,
    cost: f32,
    description: String,
    flags: ItemFlags,
    name: String,
    position: Point3D,
    weight: f32,
}

impl Item {
    game_object_copy_prop!(pub, cost, set_cost, f32);
    game_object_copy_prop!(pub, flags, set_flags, ItemFlags);
    game_object_ref_prop!(pub, position, set_position, Point3D);
    game_object_copy_prop!(pub, weight, set_weight, f32);

    pub fn has_flags(&self, flags: ItemFlags) -> bool {
        self.flags & flags == flags
    }

    pub fn hidden(&self) -> bool {
        self.has_flags(ItemFlags::Hidden)
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<SharedGameObject, String> {
        match serde_json::from_str::<ItemDto>(json) {
            Ok(item_dto) => Ok(SharedGameObject::new(Self {
                id,
                cost: item_dto.cost,
                description: item_dto.description,
                flags: item_dto.flags.unwrap_or_default(),
                name: item_dto.name,
                position: item_dto.position,
                weight: item_dto.weight,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }
}

impl fmt::Display for Item {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Item({}, {}, {})", self.id, self.name, self.position)
    }
}

impl GameObject for Item {
    game_object_string_prop!(name, set_name);
    game_object_string_prop!(description, set_description);

    fn as_item(&self) -> Option<&Self> {
        Some(&self)
    }

    fn dehydrate(&self) -> serde_json::Value {
        serde_json::to_value(ItemDto {
            cost: self.cost,
            description: self.description.clone(),
            flags: if self.flags == ItemFlags::None {
                None
            } else {
                Some(self.flags)
            },
            name: self.name.clone(),
            position: self.position.clone(),
            weight: self.weight,
        })
        .unwrap_or_else(|error| {
            panic!(
                "Failed to serialize object {:?}: {:?}",
                self.object_ref(),
                error
            )
        })
    }

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn indefinite_name(&self) -> String {
        if self.has_flags(ItemFlags::AlwaysUseDefiniteArticle) {
            format!("the {}", self.name())
        } else if self.indefinite_article().is_empty() {
            self.name().to_owned()
        } else {
            format!("{} {}", self.indefinite_article(), self.name())
        }
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Item
    }

    fn set_property(&self, realm: Realm, prop_name: &str, value: &str) -> Result<Realm, String> {
        match prop_name {
            "cost" => Ok(self.set_cost(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            "description" => Ok(self.set_description(realm, value.to_owned())),
            "flags" => Ok(self.set_flags(realm, ItemFlags::from_str(value)?)),
            "name" => Ok(self.set_name(realm, value.to_owned())),
            "position" => Ok(self.set_position(realm, Point3D::from_str(value)?)),
            "weight" => Ok(self.set_weight(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            _ => Err(format!("No property named \"{}\"", prop_name))?,
        }
    }
}

#[derive(Deserialize, Serialize)]
struct ItemDto {
    cost: f32,
    description: String,
    flags: Option<ItemFlags>,
    name: String,
    position: Point3D,
    weight: f32,
}
