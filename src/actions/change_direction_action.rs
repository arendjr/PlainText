use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::vector3d::Vector3D;

/// Changes the character's direction.
pub fn change_direction(
    realm: Realm,
    character_ref: GameObjectRef,
    direction: Vector3D,
    _: &mut Vec<PlayerOutput>,
) -> Realm {
    let character = unwrap_or_return!(realm.character(character_ref), realm);

    realm.set_shared_object(character_ref, character.with_direction(direction))
}
