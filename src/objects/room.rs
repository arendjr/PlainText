#![allow(non_upper_case_globals)]

use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::events::{EventMultiplierMap, EventType};
use crate::game_object::{
    ref_difference, ref_union, GameObject, GameObjectId, GameObjectPersistence, GameObjectRef,
    GameObjectType, SharedGameObject,
};
use crate::point3d::Point3D;

use super::Realm;

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
    game_object_ref_prop!(
        pub,
        characters,
        set_characters,
        Vec<GameObjectRef>,
        GameObjectPersistence::DontSync
    );
    game_object_ref_prop!(
        pub,
        event_multipliers,
        set_event_multipliers,
        EventMultiplierMap
    );
    game_object_copy_prop!(pub, flags, set_flags, RoomFlags);
    game_object_ref_prop!(pub, items, set_items, Vec<GameObjectRef>);
    game_object_ref_prop!(pub, portals, set_portals, Vec<GameObjectRef>);
    game_object_ref_prop!(pub, position, set_position, Point3D);

    pub fn add_characters(&self, realm: Realm, characters: Vec<GameObjectRef>) -> Realm {
        self.set_characters(realm, ref_union(&self.characters, &characters))
    }

    pub fn event_multiplier(&self, event_type: EventType) -> f32 {
        self.event_multipliers.get(event_type)
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

    pub fn remove_characters(&self, realm: Realm, characters: Vec<GameObjectRef>) -> Realm {
        self.set_characters(realm, ref_difference(&self.characters, &characters))
    }
}

impl fmt::Display for Room {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Room({}, {}, {})", self.id, self.name, self.position)
    }
}

impl GameObject for Room {
    game_object_string_prop!(name, set_name);
    game_object_string_prop!(description, set_description);

    fn as_room(&self) -> Option<&Self> {
        Some(&self)
    }

    fn dehydrate(&self) -> serde_json::Value {
        serde_json::to_value(RoomDto {
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

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Room
    }

    fn set_property(&self, realm: Realm, prop_name: &str, value: &str) -> Result<Realm, String> {
        match prop_name {
            "characters" => Ok(self.set_characters(realm, GameObjectRef::vec_from_str(value)?)),
            "description" => Ok(self.set_description(realm, value.to_owned())),
            "flags" => Ok(self.set_flags(realm, RoomFlags::from_str(value)?)),
            "items" => Ok(self.set_items(realm, GameObjectRef::vec_from_str(value)?)),
            "name" => Ok(self.set_name(realm, value.to_owned())),
            "portals" => Ok(self.set_portals(realm, GameObjectRef::vec_from_str(value)?)),
            "position" => Ok(self.set_position(realm, Point3D::from_str(value)?)),
            _ => Err(format!("No property named \"{}\"", prop_name))?,
        }
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
