#![allow(non_upper_case_globals)]

use crate::{
    entity::{
        ref_difference, ref_union, Entity, EntityId, EntityPersistence, EntityRef, EntityType,
    },
    entity_copy_prop, entity_ref_prop, entity_string_prop,
    events::{EventMultiplierMap, EventType},
    point3d::Point3D,
};
use serde::{Deserialize, Serialize};

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

#[derive(Clone, Debug, Deserialize, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Room {
    #[serde(skip)]
    id: EntityId,
    #[serde(skip)]
    characters: Vec<EntityRef>,
    description: String,
    #[serde(default, skip_serializing_if = "EventMultiplierMap::is_default")]
    event_multipliers: EventMultiplierMap,
    flags: RoomFlags,
    #[serde(default, skip_serializing_if = "Vec::is_empty")]
    items: Vec<EntityRef>,
    name: String,
    #[serde(skip)]
    needs_sync: bool,
    portals: Vec<EntityRef>,
    position: Point3D,
}

impl Room {
    entity_ref_prop!(
        pub,
        characters,
        set_characters,
        Vec<EntityRef>,
        EntityPersistence::DontSync
    );
    entity_ref_prop!(
        pub,
        event_multipliers,
        set_event_multipliers,
        EventMultiplierMap
    );
    entity_copy_prop!(pub, flags, set_flags, RoomFlags);
    entity_ref_prop!(pub, items, set_items, Vec<EntityRef>);
    entity_ref_prop!(pub, portals, set_portals, Vec<EntityRef>);
    entity_ref_prop!(pub, position, set_position, Point3D);

    pub fn add_characters(&mut self, characters: &[EntityRef]) {
        self.set_characters(ref_union(&self.characters, characters))
    }

    pub fn add_items(&mut self, items: &[EntityRef]) {
        self.set_items(ref_union(&self.items, items))
    }

    pub fn event_multiplier(&self, event_type: EventType) -> f32 {
        self.event_multipliers.get(event_type)
    }

    pub fn has_flags(&self, flags: RoomFlags) -> bool {
        self.flags & flags == flags
    }

    pub fn hydrate(id: EntityId, json: &str) -> Result<Box<dyn Entity>, String> {
        let mut room = serde_json::from_str::<Room>(json)
            .map_err(|error| format!("parse error: {}", error))?;
        room.id = id;
        Ok(Box::new(room))
    }

    pub fn remove_characters(&mut self, characters: &[EntityRef]) {
        self.set_characters(ref_difference(&self.characters, characters))
    }
}

impl Entity for Room {
    entity_string_prop!(name, set_name);
    entity_string_prop!(description, set_description);

    fn as_room(&self) -> Option<&Self> {
        Some(self)
    }

    fn as_room_mut(&mut self) -> Option<&mut Self> {
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
        EntityRef::new(EntityType::Room, self.id)
    }

    fn id(&self) -> EntityId {
        self.id
    }

    fn needs_sync(&self) -> bool {
        self.needs_sync
    }

    fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "characters" => self.set_characters(EntityRef::vec_from_str(value)?),
            "description" => self.set_description(value.to_owned()),
            "flags" => self.set_flags(RoomFlags::from_str(value)?),
            "items" => self.set_items(EntityRef::vec_from_str(value)?),
            "name" => self.set_name(value.to_owned()),
            "portals" => self.set_portals(EntityRef::vec_from_str(value)?),
            "position" => self.set_position(Point3D::from_str(value)?),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }

    fn to_json_value(&self) -> serde_json::Result<serde_json::Value> {
        serde_json::to_value(self)
    }
}
