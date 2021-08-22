#![allow(non_upper_case_globals)]

use serde::{Deserialize, Serialize};
use std::fmt;

use crate::game_object::{GameObject, GameObjectId, GameObjectType};
use crate::point3d::Point3D;

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
    indefinite_article: String,
    name: String,
    needs_sync: bool,
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

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Box<dyn GameObject>, String> {
        match serde_json::from_str::<ItemDto>(json) {
            Ok(item_dto) => Ok(Box::new(Self {
                id,
                cost: item_dto.cost,
                description: item_dto.description,
                flags: item_dto.flags.unwrap_or_default(),
                indefinite_article: item_dto.indefinite_article.unwrap_or_default(),
                name: item_dto.name,
                needs_sync: false,
                position: item_dto.position,
                weight: item_dto.weight,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn set_indefinite_article(&mut self, indefinite_article: String) {
        self.indefinite_article = indefinite_article;
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
        Some(self)
    }

    fn as_item_mut(&mut self) -> Option<&mut Self> {
        Some(self)
    }

    fn as_object(&self) -> Option<&dyn GameObject> {
        Some(self)
    }

    fn as_object_mut(&mut self) -> Option<&mut dyn GameObject> {
        Some(self)
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
            indefinite_article: if self.indefinite_article.is_empty() {
                None
            } else {
                Some(self.indefinite_article.clone())
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

    fn indefinite_article(&self) -> &str {
        &self.indefinite_article
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

    fn needs_sync(&self) -> bool {
        self.needs_sync
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Item
    }

    fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "cost" => self.set_cost(value.parse().map_err(|error| format!("{:?}", error))?),
            "description" => self.set_description(value.to_owned()),
            "flags" => self.set_flags(ItemFlags::from_str(value)?),
            "indefiniteArticle" => self.set_indefinite_article(value.to_owned()),
            "name" => self.set_name(value.to_owned()),
            "position" => self.set_position(Point3D::from_str(value)?),
            "weight" => self.set_weight(value.parse().map_err(|error| format!("{:?}", error))?),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }
}

#[derive(Deserialize, Serialize)]
#[serde(rename_all = "camelCase")]
struct ItemDto {
    cost: f32,
    description: String,
    #[serde(skip_serializing_if = "Option::is_none")]
    flags: Option<ItemFlags>,
    #[serde(skip_serializing_if = "Option::is_none")]
    indefinite_article: Option<String>,
    name: String,
    position: Point3D,
    weight: f32,
}
