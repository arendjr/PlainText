use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;
use std::sync::Arc;

use crate::game_object::{GameObject, GameObjectId, GameObjectType};
use crate::point3d::Point3D;

#[derive(Debug)]
pub struct Item {
    id: GameObjectId,
    cost: f32,
    description: String,
    name: String,
    position: Point3D,
    weight: f32,
}

impl Item {
    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Arc<dyn GameObject>, String> {
        match serde_json::from_str::<ItemDto>(json) {
            Ok(item_dto) => Ok(Arc::new(Item {
                id,
                cost: item_dto.cost,
                description: item_dto.description,
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
    fn get_id(&self) -> GameObjectId {
        self.id
    }

    fn get_object_type(&self) -> GameObjectType {
        GameObjectType::Item
    }

    fn get_name(&self) -> String {
        self.name.clone()
    }

    fn to_item(&self) -> Option<&Item> {
        Some(self)
    }
}

#[derive(Deserialize, Serialize)]
struct ItemDto {
    cost: f32,
    description: String,
    name: String,
    position: Point3D,
    weight: f32,
}
