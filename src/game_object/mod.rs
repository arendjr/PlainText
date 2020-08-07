mod game_object;
mod game_object_map;
mod game_object_ref;
mod game_object_util;

pub use game_object::{hydrate, GameObject};
pub use game_object_map::{
    new as new_map, GameObjectMap, GameObjectMapReader, GameObjectMapWriter,
};
pub use game_object_ref::{GameObjectId, GameObjectRef, GameObjectType};
pub use game_object_util::{ref_difference, ref_union};
