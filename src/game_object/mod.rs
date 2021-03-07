mod character;
#[macro_use]
mod game_object;
mod game_object_ref;
mod game_object_util;
mod shared_game_object;

pub use character::{Character, Gender};
pub use game_object::{hydrate, GameObject, GameObjectPersistence};
pub use game_object_ref::{GameObjectId, GameObjectRef, GameObjectType};
pub use game_object_util::{ref_difference, ref_union};
pub use shared_game_object::SharedGameObject;
