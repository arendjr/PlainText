use crate::{
    entity::{EntityRef, Realm},
    vector3d::Vector3D,
};

/// Changes the character's direction.
pub fn change_direction(realm: &mut Realm, character_ref: EntityRef, direction: Vector3D) {
    if let Some(character) = realm.character_mut(character_ref) {
        character.set_direction(direction);
    }
}
