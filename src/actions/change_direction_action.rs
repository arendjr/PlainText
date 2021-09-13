use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::vector3d::Vector3D;

/// Changes the character's direction.
pub fn change_direction(realm: &mut Realm, character_ref: GameObjectRef, direction: Vector3D) {
    if let Some(character) = realm.character_mut(character_ref) {
        character.set_direction(direction);
    }
}
