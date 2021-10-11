#![allow(non_upper_case_globals)]

use crate::{
    entity::{Entity, EntityId, EntityRef, EntityType},
    entity_copy_prop, entity_ref_prop, entity_string_prop,
    point3d::Point3D,
};
use serde::{Deserialize, Serialize};

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

#[derive(Clone, Debug, Deserialize, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Item {
    #[serde(skip)]
    id: EntityId,
    cost: f32,
    description: String,
    #[serde(default, skip_serializing_if = "ItemFlags::is_default")]
    flags: ItemFlags,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    indefinite_article: String,
    name: String,
    #[serde(skip)]
    needs_sync: bool,
    position: Point3D,
    weight: f32,
}

impl Item {
    entity_copy_prop!(pub, cost, set_cost, f32);
    entity_copy_prop!(pub, flags, set_flags, ItemFlags);
    entity_ref_prop!(pub, position, set_position, Point3D);
    entity_copy_prop!(pub, weight, set_weight, f32);

    pub fn has_flags(&self, flags: ItemFlags) -> bool {
        self.flags & flags == flags
    }

    pub fn hidden(&self) -> bool {
        self.has_flags(ItemFlags::Hidden)
    }

    pub fn hydrate(id: EntityId, json: &str) -> Result<Box<dyn Entity>, String> {
        let mut item = serde_json::from_str::<Item>(json)
            .map_err(|error| format!("parse error: {}", error))?;
        item.id = id;
        Ok(Box::new(item))
    }

    pub fn set_indefinite_article(&mut self, indefinite_article: String) {
        self.indefinite_article = indefinite_article;
    }
}

impl Entity for Item {
    entity_string_prop!(name, set_name);
    entity_string_prop!(description, set_description);

    fn as_item(&self) -> Option<&Self> {
        Some(self)
    }

    fn as_item_mut(&mut self) -> Option<&mut Self> {
        Some(self)
    }

    fn as_entity(&self) -> Option<&dyn Entity> {
        Some(self)
    }

    fn as_entity_mut(&mut self) -> Option<&mut dyn Entity> {
        Some(self)
    }

    fn dehydrate(&self) -> String {
        serde_json::to_string_pretty(self).unwrap_or_else(|error| {
            panic!(
                "Failed to serialize entity {:?}: {:?}",
                self.entity_ref(),
                error
            )
        })
    }

    fn entity_ref(&self) -> EntityRef {
        EntityRef::new(EntityType::Item, self.id)
    }

    fn id(&self) -> EntityId {
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

    fn to_json_value(&self) -> serde_json::Result<serde_json::Value> {
        serde_json::to_value(self)
    }
}
