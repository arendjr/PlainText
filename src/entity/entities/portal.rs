#![allow(non_upper_case_globals)]

use crate::{
    entity::{Entity, EntityId, EntityRef, EntityType, Openable, Realm},
    entity_copy_prop, entity_string_prop,
    events::{EventMultiplierMap, EventType},
    point3d::Point3D,
    serializable_flags,
};
use serde::{Deserialize, Serialize};

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
    }
}

#[derive(Clone, Debug, Deserialize, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Portal {
    #[serde(skip)]
    id: EntityId,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    description: String,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    description2: String,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    destination: String,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    destination2: String,
    #[serde(default, skip_serializing_if = "EventMultiplierMap::is_default")]
    event_multipliers: EventMultiplierMap,
    #[serde(default, skip_serializing_if = "PortalFlags::is_default")]
    flags: PortalFlags,
    name: String,
    name2: String,
    #[serde(skip)]
    needs_sync: bool,
    #[serde(skip_serializing_if = "Option::is_none")]
    openable: Option<Box<Openable>>,
    room: EntityRef,
    room2: EntityRef,
}

impl Portal {
    entity_string_prop!(pub, description2, set_description2);
    entity_string_prop!(pub, destination, set_destination);
    entity_string_prop!(pub, destination2, set_destination2);
    entity_string_prop!(pub, name2, set_name2);
    entity_copy_prop!(pub, room, set_room, EntityRef);
    entity_copy_prop!(pub, room2, set_room2, EntityRef);

    pub fn can_hear_through(&self) -> bool {
        if self.is_open() {
            self.has_flags(PortalFlags::CanHearThroughIfOpen)
        } else {
            self.has_flags(PortalFlags::CanHearThrough)
        }
    }

    pub fn can_open(&self) -> bool {
        self.has_flags(PortalFlags::CanOpenFromSide1)
            || self.has_flags(PortalFlags::CanOpenFromSide2)
    }

    pub fn can_open_from_room(&self, room: EntityRef) -> bool {
        if room == self.room2 {
            self.has_flags(PortalFlags::CanOpenFromSide2)
        } else {
            self.has_flags(PortalFlags::CanOpenFromSide1)
        }
    }

    pub fn can_pass_through(&self) -> bool {
        if self.is_open() {
            self.has_flags(PortalFlags::CanPassThroughIfOpen)
        } else {
            self.has_flags(PortalFlags::CanPassThrough)
        }
    }

    pub fn can_see_through(&self) -> bool {
        if self.is_open() {
            self.has_flags(PortalFlags::CanSeeThroughIfOpen)
        } else {
            self.has_flags(PortalFlags::CanSeeThrough)
        }
    }

    pub fn destination_from_room(&self, room: EntityRef) -> &str {
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

    pub fn hydrate(id: EntityId, json: &str) -> Result<Box<dyn Entity>, String> {
        let mut portal = serde_json::from_str::<Portal>(json)
            .map_err(|error| format!("parse error: {}", error))?;
        portal.id = id;
        if portal.can_open() && portal.openable.is_none() {
            portal.openable = Some(Box::new(Openable::new()));
        }
        Ok(Box::new(portal))
    }

    pub fn is_hidden_from_room(&self, room: EntityRef) -> bool {
        self.has_flags(if room == self.room2 {
            PortalFlags::IsHiddenFromSide2
        } else {
            PortalFlags::IsHiddenFromSide1
        })
    }

    pub fn is_open(&self) -> bool {
        self.openable
            .as_ref()
            .map(|openable| openable.is_open())
            .unwrap_or(false)
    }

    pub fn name_from_room(&self, room: EntityRef) -> &str {
        if room == self.room2 && !self.name2.is_empty() {
            &self.name2
        } else {
            &self.name
        }
    }

    /// Returns a minimal description of the portal composed of the portal's name and its
    /// destination.
    pub fn name_with_destination_from_room(&self, room: EntityRef) -> String {
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

    pub fn opposite_of(&self, room: EntityRef) -> EntityRef {
        if room == self.room2 {
            self.room
        } else {
            self.room2
        }
    }

    pub fn position(&self, realm: &Realm) -> Point3D {
        let position1 = match realm.room(self.room) {
            Some(room) => room.position(),
            None => return Point3D::default(),
        };
        let position2 = match realm.room(self.room2) {
            Some(room) => room.position(),
            None => return Point3D::default(),
        };
        Point3D {
            x: (position1.x + position2.x) / 2,
            y: (position1.y + position2.y) / 2,
            z: (position1.z + position2.z) / 2,
        }
    }

    pub fn set_flags(&mut self, flags: PortalFlags) {
        self.flags = flags;
        self.needs_sync = true;

        if self.can_open() {
            if self.openable.is_none() {
                self.openable = Some(Box::new(Openable::new()));
            }
        } else if let Some(openable) = self.openable.as_mut() {
            if openable.auto_close_message().is_empty() && openable.auto_close_timeout().is_none() {
                self.openable = None;
            } else {
                openable.set_open(false);
            }
        }
    }

    pub fn set_open(&mut self, is_open: bool) {
        if let Some(openable) = self.openable.as_mut() {
            openable.set_open(is_open);
        } else if is_open {
            let mut openable = Openable::new();
            openable.set_open(is_open);
            self.openable = Some(Box::new(openable));
        }
    }
}

impl Entity for Portal {
    entity_string_prop!(name, set_name);
    entity_string_prop!(description, set_description);

    fn as_entity(&self) -> Option<&dyn Entity> {
        Some(self)
    }

    fn as_entity_mut(&mut self) -> Option<&mut dyn Entity> {
        Some(self)
    }

    fn as_openable(&self) -> Option<&Openable> {
        self.openable.as_ref().map(Box::as_ref)
    }

    fn as_openable_mut(&mut self) -> Option<&mut Openable> {
        self.openable.as_mut().map(Box::as_mut)
    }

    fn as_portal(&self) -> Option<&Self> {
        Some(self)
    }

    fn as_portal_mut(&mut self) -> Option<&mut Self> {
        Some(self)
    }

    fn dehydrate(&self) -> String {
        serde_json::to_string_pretty(self).unwrap_or_else(|error| {
            panic!(
                "Failed to serialize entity {:?}: {:?}",
                self.entity_ref(),
                error
            )
        })
    }

    fn entity_ref(&self) -> EntityRef {
        EntityRef::new(EntityType::Portal, self.id)
    }

    fn id(&self) -> EntityId {
        self.id
    }

    fn needs_sync(&self) -> bool {
        self.needs_sync
            || self
                .openable
                .as_ref()
                .map(|openable| openable.needs_sync())
                .unwrap_or(false)
    }

    fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;

        if !needs_sync {
            if let Some(openable) = self.openable.as_mut() {
                openable.set_needs_sync(needs_sync);
            }
        }
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "description" => self.set_description(value.to_owned()),
            "description2" => self.set_description2(value.to_owned()),
            "destination" => self.set_destination(value.to_owned()),
            "destination2" => self.set_destination2(value.to_owned()),
            "flags" => self.set_flags(PortalFlags::from_str(value)?),
            "name" => self.set_name(value.to_owned()),
            "name2" => self.set_name2(value.to_owned()),
            "room" => self.set_room(EntityRef::from_str(value)?),
            "room2" => self.set_room2(EntityRef::from_str(value)?),
            _ => match self.openable.as_mut() {
                Some(openable) => openable.set_property(prop_name, value),
                None => Err(format!("No property named \"{}\"", prop_name)),
            }?,
        }
        Ok(())
    }

    fn to_json_value(&self) -> serde_json::Result<serde_json::Value> {
        serde_json::to_value(self)
    }
}
