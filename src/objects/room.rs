use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::events::{EventMultiplierMap, EventType};
use crate::game_object::{
    ref_difference, ref_union, GameObject, GameObjectId, GameObjectRef, GameObjectType,
    SharedGameObject,
};
use crate::point3d::Point3D;

serializable_flags! {
    struct RoomFlags: u32 {
        const DistantCharacterDescriptions = 0b00000001;
        const DynamicPortalDescriptions    = 0b00000010;
        const HasCeiling                   = 0b00000100;
        const HasFloor                     = 0b00001000;
        const HasWalls                     = 0b00010000;
        const IsRoad                       = 0b00100000;
        const IsRiver                      = 0b01000000;
        const IsRoof                       = 0b10000000;
    }
}

#[derive(Clone, Debug)]
pub struct Room {
    id: GameObjectId,
    characters: Vec<GameObjectRef>,
    description: String,
    event_multipliers: EventMultiplierMap,
    flags: RoomFlags,
    items: Vec<GameObjectRef>,
    name: String,
    portals: Vec<GameObjectRef>,
    position: Point3D,
}

impl Room {
    pub fn characters(&self) -> &Vec<GameObjectRef> {
        &self.characters
    }

    pub fn description(&self) -> &str {
        &self.description
    }

    pub fn event_multiplier(&self, event_type: EventType) -> f32 {
        self.event_multipliers.get(event_type)
    }

    pub fn flags(&self) -> RoomFlags {
        self.flags
    }

    pub fn has_flags(&self, flags: RoomFlags) -> bool {
        self.flags & flags == flags
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<SharedGameObject, String> {
        match serde_json::from_str::<RoomDto>(json) {
            Ok(room_dto) => Ok(SharedGameObject::new(Self {
                id,
                characters: vec![],
                description: room_dto.description,
                event_multipliers: room_dto.eventMultipliers.unwrap_or_default(),
                flags: room_dto.flags,
                items: room_dto.items.unwrap_or_default(),
                name: room_dto.name,
                portals: room_dto.portals,
                position: room_dto.position,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn items(&self) -> &Vec<GameObjectRef> {
        &self.items
    }

    pub fn portals(&self) -> &Vec<GameObjectRef> {
        &self.portals
    }

    pub fn position(&self) -> &Point3D {
        &self.position
    }

    pub fn with_characters(&self, characters: Vec<GameObjectRef>) -> (Self, bool) {
        (
            Self {
                characters: ref_union(&self.characters, &characters),
                ..self.clone()
            },
            false,
        )
    }

    pub fn without_characters(&self, characters: Vec<GameObjectRef>) -> (Self, bool) {
        (
            Self {
                characters: ref_difference(&self.characters, &characters),
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
            eventMultipliers: Some(self.event_multipliers.clone()),
            flags: self.flags,
            items: if self.items.is_empty() {
                None
            } else {
                Some(self.items.clone())
            },
            name: self.name.clone(),
            portals: self.portals().clone(),
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

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct RoomDto {
    description: String,
    eventMultipliers: Option<EventMultiplierMap>,
    flags: RoomFlags,
    items: Option<Vec<GameObjectRef>>,
    name: String,
    portals: Vec<GameObjectRef>,
    position: Point3D,
}
