use evmap;
use std::collections::HashMap;
use std::hash::{Hash, Hasher};
use std::sync::{Arc, Mutex};

use crate::objects;

use super::{GameObject, GameObjectId, GameObjectRef, GameObjectType};

impl Eq for dyn GameObject {}

impl Hash for dyn GameObject {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.get_id().hash(state);
        self.get_object_type().hash(state);
    }
}

impl PartialEq for dyn GameObject {
    fn eq(&self, other: &Self) -> bool {
        self.get_id() == other.get_id()
    }
}

#[derive(Clone)]
pub struct GameObjectMapReader {
    handle: evmap::ReadHandle<GameObjectRef, Arc<dyn GameObject>>,
    player_id_map: Arc<Mutex<HashMap<String, GameObjectId>>>,
}

impl GameObjectMapReader {
    pub fn get(&self, object_ref: GameObjectRef) -> Option<evmap::ReadGuard<Arc<dyn GameObject>>> {
        self.handle.get_one(&object_ref)
    }

    pub fn get_item(&self, id: GameObjectId) -> Option<objects::Item> {
        self.get(GameObjectRef(GameObjectType::Item, id))
            .and_then(|object| object.to_item())
    }

    pub fn get_player(&self, id: GameObjectId) -> Option<objects::Player> {
        self.get(GameObjectRef(GameObjectType::Player, id))
            .and_then(|object| object.to_player())
    }

    pub fn get_player_by_name(self, name: &str) -> Option<objects::Player> {
        match self.player_id_map.lock() {
            Ok(map) => map
                .get(name)
                .and_then(|id| self.get(GameObjectRef(GameObjectType::Player, *id)))
                .and_then(|object| object.to_player()),
            Err(error) => panic!("Failed to acquire player mutex: {}", error),
        }
    }

    pub fn get_portal(&self, id: GameObjectId) -> Option<objects::Portal> {
        self.get(GameObjectRef(GameObjectType::Portal, id))
            .and_then(|object| object.to_portal())
    }

    pub fn get_race(&self, id: GameObjectId) -> Option<objects::Race> {
        self.get(GameObjectRef(GameObjectType::Race, id))
            .and_then(|object| object.to_race())
    }

    pub fn get_room(&self, id: GameObjectId) -> Option<objects::Room> {
        self.get(GameObjectRef(GameObjectType::Room, id))
            .and_then(|object| object.to_room())
    }
}

pub struct GameObjectMapWriter {
    handle: evmap::WriteHandle<GameObjectRef, Arc<dyn GameObject>>,
    player_id_map: Arc<Mutex<HashMap<String, GameObjectId>>>,
}

impl GameObjectMapWriter {
    pub fn can_set(&self, object_ref: GameObjectRef, object: Arc<dyn GameObject>) -> bool {
        if object.get_revision_num() == 0 {
            if self.handle.contains_key(&object_ref) {
                println!("Object {} already inserted", object_ref);
                false
            } else {
                true
            }
        } else if let Some(existing_object) = self.handle.get_one(&object_ref) {
            if object.get_revision_num() == existing_object.get_revision_num() + 1 {
                true
            } else {
                println!(
                    "Object {} at revision {}; attempt to update to revision {} rejected",
                    object_ref,
                    existing_object.get_revision_num(),
                    object.get_revision_num()
                );
                false
            }
        } else {
            println!(
                "Object {} already got deleted (or was never properly inserted)",
                object_ref
            );
            false
        }
    }

    pub fn has(&self, object_ref: &GameObjectRef) -> bool {
        self.handle.contains_key(object_ref)
    }

    pub fn refresh(&mut self) {
        self.handle.refresh();
    }

    pub fn register_player(&self, player: Arc<dyn GameObject>) -> bool {
        match self.player_id_map.try_lock() {
            Ok(mut map) => match map.get(&player.get_name()) {
                Some(_) => false, // Player with that name already registered.
                None => {
                    map.insert(player.get_name(), player.get_id());
                    true
                }
            },
            Err(error) => panic!("Failed to acquire player mutex: {}", error),
        }
    }

    // Always call `can_set()` first for validation.
    pub fn set(&mut self, object_ref: GameObjectRef, object: Arc<dyn GameObject>) {
        self.handle.update(object_ref, object);
    }

    pub fn unset(&mut self, object_ref: GameObjectRef) {
        self.handle.empty(object_ref);
    }
}

pub type GameObjectMap = (GameObjectMapReader, GameObjectMapWriter);

pub fn new() -> GameObjectMap {
    let (read_handle, write_handle) = evmap::new();
    let player_id_map = Arc::new(Mutex::new(HashMap::new()));

    (
        GameObjectMapReader {
            handle: read_handle,
            player_id_map: player_id_map.clone(),
        },
        GameObjectMapWriter {
            handle: write_handle,
            player_id_map,
        },
    )
}
