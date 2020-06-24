use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;
use std::sync::Arc;

use crate::game_object::{GameObject, GameObjectId, GameObjectRef, GameObjectType};

#[derive(Debug)]
pub struct Portal {
    id: GameObjectId,
    description: String,
    description2: String,
    name: String,
    name2: String,
    room: GameObjectRef,
    room2: GameObjectRef,
}

impl Portal {
    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Arc<dyn GameObject>, String> {
        match serde_json::from_str::<PortalDto>(json) {
            Ok(portal_dto) => Ok(Arc::new(Portal {
                id,
                description: match portal_dto.description {
                    Some(description) => description,
                    None => "".to_owned(),
                },
                description2: match portal_dto.description2 {
                    Some(description) => description,
                    None => "".to_owned(),
                },
                name: portal_dto.name,
                name2: portal_dto.name2,
                room: portal_dto.room,
                room2: portal_dto.room2,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }
}

impl fmt::Display for Portal {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Portal({}, {}, {})", self.id, self.name, self.name2)
    }
}

impl GameObject for Portal {
    fn get_id(&self) -> GameObjectId {
        self.id
    }

    fn get_object_type(&self) -> GameObjectType {
        GameObjectType::Portal
    }

    fn get_name(&self) -> String {
        self.name.clone()
    }

    fn to_portal(&self) -> Option<&Portal> {
        Some(self)
    }
}

#[derive(Deserialize, Serialize)]
struct PortalDto {
    description: Option<String>,
    description2: Option<String>,
    name: String,
    name2: String,
    room: GameObjectRef,
    room2: GameObjectRef,
}
