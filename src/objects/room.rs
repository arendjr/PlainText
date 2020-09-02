use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;
use std::ops::Deref;
use std::sync::Arc;

use crate::game_object::{
    ref_difference, ref_union, GameObject, GameObjectId, GameObjectRef, GameObjectType,
    SharedGameObject,
};
use crate::point3d::Point3D;

#[derive(Clone, Debug)]
pub struct Room {
    id: GameObjectId,
    characters: Arc<Vec<GameObjectRef>>,
    description: String,
    items: Arc<Vec<GameObjectRef>>,
    name: String,
    portals: Arc<Vec<GameObjectRef>>,
    position: Point3D,
}

impl Room {
    pub fn description(&self) -> &str {
        &self.description
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<SharedGameObject, String> {
        match serde_json::from_str::<RoomDto>(json) {
            Ok(room_dto) => Ok(SharedGameObject::new(Self {
                id,
                characters: Arc::new(vec![]),
                description: room_dto.description,
                items: Arc::new(match room_dto.items {
                    Some(items) => items,
                    None => vec![],
                }),
                name: room_dto.name,
                portals: Arc::new(room_dto.portals),
                position: room_dto.position,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn portals(&self) -> Arc<Vec<GameObjectRef>> {
        self.portals.clone()
    }

    pub fn with_characters(&self, characters: Vec<GameObjectRef>) -> (Self, bool) {
        (
            Self {
                characters: Arc::new(ref_union(&self.characters, &characters)),
                ..self.clone()
            },
            false,
        )
    }

    pub fn without_characters(&self, characters: Vec<GameObjectRef>) -> (Self, bool) {
        (
            Self {
                characters: Arc::new(ref_difference(&self.characters, &characters)),
                ..self.clone()
            },
            false,
        )
    }
}

impl fmt::Display for Room {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Room({}, {}, {})", self.id, self.name, self.position)
    }
}

impl GameObject for Room {
    fn as_room(&self) -> Option<&Self> {
        Some(&self)
    }

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Room
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn serialize(&self) -> String {
        serde_json::to_string_pretty(&RoomDto {
            description: self.description.clone(),
            items: if self.items.is_empty() {
                None
            } else {
                Some(self.items.deref().clone())
            },
            name: self.name.clone(),
            portals: self.portals().deref().clone(),
            position: self.position.clone(),
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
struct RoomDto {
    description: String,
    items: Option<Vec<GameObjectRef>>,
    name: String,
    portals: Vec<GameObjectRef>,
    position: Point3D,
}
