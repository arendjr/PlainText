use crate::entity::{Entity, EntityRef, Realm};

pub fn named_portal(realm: &Realm, room_ref: EntityRef, name: &str) -> Option<EntityRef> {
    realm.room(room_ref).and_then(|room| {
        room.portals()
            .iter()
            .filter_map(|portal_ref| realm.portal(*portal_ref))
            .find(|portal| portal.name_from_room(room_ref) == name)
            .map(|portal| portal.entity_ref())
    })
}
