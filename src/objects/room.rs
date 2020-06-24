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
    characters: Vec<GameObjectRef>,
    description: String,
    items: Vec<GameObjectRef>,
    name: String,
    portals: Vec<GameObjectRef>,
    position: Point3D,
}

impl Room {
    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Arc<dyn GameObject>, String> {
        match serde_json::from_str::<RoomDto>(json) {
            Ok(room_dto) => Ok(Arc::new(Room {
                id,
                characters: vec![],
                description: room_dto.description,
                items: match room_dto.items {
                    Some(items) => items,
                    None => vec![],
                },
                name: room_dto.name,
                portals: room_dto.portals,
                position: room_dto.position,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn with_characters(&self, characters: Vec<GameObjectRef>) -> Room {
        Room {
            characters: ref_union(&self.characters, &characters),
            ..self.clone()
        }
    }

    pub fn without_characters(&self, characters: Vec<GameObjectRef>) -> Room {
        Room {
            characters: ref_difference(&self.characters, &characters),
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

    fn to_room(&self) -> Option<&Room> {
        Some(self)
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
