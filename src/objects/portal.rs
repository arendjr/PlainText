#![allow(non_upper_case_globals)]

use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::events::{EventMultiplierMap, EventType};
use crate::game_object::{
    GameObject, GameObjectId, GameObjectRef, GameObjectType, SharedGameObject,
};
use crate::objects::Realm;
use crate::point3d::Point3D;

serializable_flags! {
    pub struct PortalFlags: u32 {
        const IsHiddenFromSide1     = 0b0000000000000001;
        const IsHiddenFromSide2     = 0b0000000000000010;
        const CanOpenFromSide1      = 0b0000000000000100;
        const CanOpenFromSide2      = 0b0000000000001000;
        const CanSeeThrough         = 0b0000000000010000;
        const CanHearThrough        = 0b0000000000100000;
        const CanShootThrough       = 0b0000000001000000;
        const CanPassThrough        = 0b0000000010000000;
        const CanSeeThroughIfOpen   = 0b0000000100000000;
        const CanHearThroughIfOpen  = 0b0000001000000000;
        const CanShootThroughIfOpen = 0b0000010000000000;
        const CanPassThroughIfOpen  = 0b0000100000000000;
        const IsOpen                = 0b0001000000000000;
    }
}

#[derive(Clone, Debug)]
pub struct Portal {
    id: GameObjectId,
    description: String,
    description2: String,
    destination: String,
    destination2: String,
    event_multipliers: EventMultiplierMap,
    flags: PortalFlags,
    name: String,
    name2: String,
    room: GameObjectRef,
    room2: GameObjectRef,
}

impl Portal {
    game_object_string_prop!(pub, description2, set_description2);
    game_object_string_prop!(pub, destination, set_destination);
    game_object_string_prop!(pub, destination2, set_destination2);
    game_object_copy_prop!(pub, flags, set_flags, PortalFlags);
    game_object_string_prop!(pub, name2, set_name2);
    game_object_copy_prop!(pub, room, set_room, GameObjectRef);
    game_object_copy_prop!(pub, room2, set_room2, GameObjectRef);

    pub fn can_see_through(&self) -> bool {
        if self.has_flags(PortalFlags::IsOpen) {
            self.has_flags(PortalFlags::CanSeeThroughIfOpen)
        } else {
            self.has_flags(PortalFlags::CanSeeThrough)
        }
    }

    pub fn destination_from_room(&self, room: GameObjectRef) -> &str {
        if room == self.room2 {
            &self.destination2
        } else {
            &self.destination
        }
    }

    pub fn event_multiplier(&self, event_type: EventType) -> f32 {
        self.event_multipliers.get(event_type)
    }

    pub fn has_flags(&self, flags: PortalFlags) -> bool {
        self.flags & flags == flags
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<SharedGameObject, String> {
        match serde_json::from_str::<PortalDto>(json) {
            Ok(portal_dto) => Ok(SharedGameObject::new(Self {
                id,
                description: portal_dto.description.unwrap_or_default(),
                description2: portal_dto.description2.unwrap_or_default(),
                destination: portal_dto.destination.unwrap_or_default(),
                destination2: portal_dto.destination2.unwrap_or_default(),
                event_multipliers: portal_dto.eventMultipliers.unwrap_or_default(),
                flags: portal_dto.flags,
                name: portal_dto.name,
                name2: portal_dto.name2,
                room: portal_dto.room,
                room2: portal_dto.room2,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn is_hidden_from_room(&self, room: GameObjectRef) -> bool {
        self.has_flags(if room == self.room2 {
            PortalFlags::IsHiddenFromSide2
        } else {
            PortalFlags::IsHiddenFromSide1
        })
    }

    pub fn name_from_room(&self, room: GameObjectRef) -> &str {
        if room == self.room2 && !self.name2.is_empty() {
            &self.name2
        } else {
            &self.name
        }
    }

    /// Returns a minimal description of the portal composed of the portal's name and its
    /// destination.
    pub fn name_with_destination_from_room(&self, room: GameObjectRef) -> String {
        let name = self.name_from_room(room);
        let destination = self.destination_from_room(room);
        if destination.is_empty() {
            if name == "door" || name == "tent" {
                format!("a {}", name)
            } else {
                format!("the {}", name)
            }
        } else {
            format!("the {} to {}", name, destination)
        }
    }

    pub fn opposite_of(&self, room: GameObjectRef) -> GameObjectRef {
        if room == self.room2 {
            self.room
        } else {
            self.room2
        }
    }

    pub fn position(&self, realm: &Realm) -> Point3D {
        let position1 = unwrap_or_return!(
            realm.room(self.room).map(|room| room.position()),
            Point3D::default()
        );
        let position2 = unwrap_or_return!(
            realm.room(self.room2).map(|room| room.position()),
            Point3D::default()
        );
        Point3D {
            x: (position1.x + position2.x) / 2,
            y: (position1.y + position2.y) / 2,
            z: (position1.z + position2.z) / 2,
        }
    }
}

impl fmt::Display for Portal {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Portal({}, {}, {})", self.id, self.name, self.name2)
    }
}

impl GameObject for Portal {
    game_object_string_prop!(name, set_name);
    game_object_string_prop!(description, set_description);

    fn as_portal(&self) -> Option<&Self> {
        Some(&self)
    }

    fn dehydrate(&self) -> serde_json::Value {
        serde_json::to_value(PortalDto {
            description: Some(self.description.clone()),
            description2: if self.description2.is_empty() {
                None
            } else {
                Some(self.description2.clone())
            },
            destination: if self.destination.is_empty() {
                None
            } else {
                Some(self.destination.clone())
            },
            destination2: if self.destination2.is_empty() {
                None
            } else {
                Some(self.destination2.clone())
            },
            eventMultipliers: Some(self.event_multipliers.clone()),
            flags: self.flags,
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

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Portal
    }

    fn set_property(&self, realm: Realm, prop_name: &str, value: &str) -> Result<Realm, String> {
        match prop_name {
            "description" => Ok(self.set_description(realm, value.to_owned())),
            "description2" => Ok(self.set_description2(realm, value.to_owned())),
            "destination" => Ok(self.set_destination(realm, value.to_owned())),
            "destination2" => Ok(self.set_destination2(realm, value.to_owned())),
            "flags" => Ok(self.set_flags(realm, PortalFlags::from_str(value)?)),
            "name" => Ok(self.set_name(realm, value.to_owned())),
            "name2" => Ok(self.set_name2(realm, value.to_owned())),
            "room" => Ok(self.set_room(realm, GameObjectRef::from_str(value)?)),
            "room2" => Ok(self.set_room2(realm, GameObjectRef::from_str(value)?)),
            _ => Err(format!("No property named \"{}\"", prop_name))?,
        }
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct PortalDto {
    description: Option<String>,
    description2: Option<String>,
    destination: Option<String>,
    destination2: Option<String>,
    eventMultipliers: Option<EventMultiplierMap>,
    flags: PortalFlags,
    name: String,
    name2: String,
    room: GameObjectRef,
    room2: GameObjectRef,
}
