use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;
use std::sync::Arc;

use crate::game_object::{
    ref_difference, ref_union, GameObject, GameObjectId, GameObjectRef, GameObjectType,
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
    revision_num: u32,
}

impl Room {
    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Arc<dyn GameObject>, String> {
        match serde_json::from_str::<RoomDto>(json) {
            Ok(room_dto) => Ok(Arc::new(Self {
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
                revision_num: 0,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn with_characters(&self, characters: Vec<GameObjectRef>) -> Self {
        Self {
            characters: Arc::new(ref_union(&self.characters, &characters)),
            revision_num: self.revision_num + 1,
            ..self.clone()
        }
    }

    pub fn without_characters(&self, characters: Vec<GameObjectRef>) -> Self {
        Self {
            characters: Arc::new(ref_difference(&self.characters, &characters)),
            revision_num: self.revision_num + 1,
            ..self.clone()
        }
    }
}

impl fmt::Display for Room {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Room({}, {}, {})", self.id, self.name, self.position)
    }
}

impl GameObject for Room {
    fn get_id(&self) -> GameObjectId {
        self.id
    }

    fn get_object_type(&self) -> GameObjectType {
        GameObjectType::Room
    }

    fn get_name(&self) -> String {
        self.name.clone()
    }

    fn get_revision_num(&self) -> u32 {
        self.revision_num
    }

    fn to_room(&self) -> Option<Self> {
        Some(self.clone())
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
