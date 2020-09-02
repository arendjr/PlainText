use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;
use std::sync::Arc;

use crate::game_object::{GameObject, GameObjectId, GameObjectType};
use crate::point3d::Point3D;

#[derive(Clone, Debug)]
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
            Ok(item_dto) => Ok(Arc::new(Self {
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
    fn description(&self) -> &str {
        &self.description
    }

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Item
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn to_item(&self) -> Option<Self> {
        Some(self.clone())
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
