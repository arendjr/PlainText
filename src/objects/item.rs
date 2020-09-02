use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::game_object::{GameObject, GameObjectId, GameObjectType, SharedGameObject};
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
    pub fn hydrate(id: GameObjectId, json: &str) -> Result<SharedGameObject, String> {
        match serde_json::from_str::<ItemDto>(json) {
            Ok(item_dto) => Ok(SharedGameObject::new(Self {
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
    fn as_item(&self) -> Option<&Self> {
        Some(&self)
    }

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

    fn serialize(&self) -> String {
        serde_json::to_string_pretty(&ItemDto {
            cost: self.cost,
            description: self.description.clone(),
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
}

#[derive(Deserialize, Serialize)]
struct ItemDto {
    cost: f32,
    description: String,
    name: String,
    position: Point3D,
    weight: f32,
}
