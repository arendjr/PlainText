use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::game_object::{
    GameObject, GameObjectId, GameObjectRef, GameObjectType, SharedGameObject,
};

#[derive(Clone, Debug)]
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
    pub fn name_from_room(&self, room: GameObjectRef) -> &str {
        if room == self.room2 && !self.name2.is_empty() {
            &self.name2
        } else {
            &self.name
        }
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<SharedGameObject, String> {
        match serde_json::from_str::<PortalDto>(json) {
            Ok(portal_dto) => Ok(SharedGameObject::new(Self {
                id,
                description: match portal_dto.description {
                    Some(description) => description,
                    None => String::from(""),
                },
                description2: match portal_dto.description2 {
                    Some(description) => description,
                    None => String::from(""),
                },
                name: portal_dto.name,
                name2: portal_dto.name2,
                room: portal_dto.room,
                room2: portal_dto.room2,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn opposite_of(&self, room: GameObjectRef) -> GameObjectRef {
        if room == self.room2 {
            self.room
        } else {
            self.room2
        }
    }
}

impl fmt::Display for Portal {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Portal({}, {}, {})", self.id, self.name, self.name2)
    }
}

impl GameObject for Portal {
    fn as_portal(&self) -> Option<&Self> {
        Some(&self)
    }

    fn description(&self) -> &str {
        &self.description
    }

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Portal
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn serialize(&self) -> String {
        serde_json::to_string_pretty(&PortalDto {
            description: Some(self.description.clone()),
            description2: if self.description2.is_empty() {
                None
            } else {
                Some(self.description2.clone())
            },
            name: self.name.clone(),
            name2: self.name2.clone(),
            room: self.room,
            room2: self.room2,
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
struct PortalDto {
    description: Option<String>,
    description2: Option<String>,
    name: String,
    name2: String,
    room: GameObjectRef,
    room2: GameObjectRef,
}
