use serde::{Deserialize, Serialize};
use serde_json;
use std::hash::{Hash, Hasher};
use std::sync::Arc;

use im_rc::HashMap;

use crate::objects;

use crate::game_object::{GameObject, GameObjectId, GameObjectRef, GameObjectType};

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
pub struct Realm {
    date_time: u64,
    id: GameObjectId,
    name: String,
    objects: HashMap<GameObjectRef, Arc<dyn GameObject>>,
    players_by_name: HashMap<String, GameObjectId>,
    races_by_name: HashMap<String, GameObjectId>,
}

impl Realm {
    pub fn get(&self, object_ref: GameObjectRef) -> Option<Arc<dyn GameObject>> {
        self.objects
            .get(&object_ref)
            .and_then(|object| Some(object.clone()))
    }

    pub fn get_item(&self, id: GameObjectId) -> Option<objects::Item> {
        self.get(GameObjectRef(GameObjectType::Item, id))
            .and_then(|object| object.to_item())
    }

    pub fn get_player(&self, id: GameObjectId) -> Option<objects::Player> {
        self.get(GameObjectRef(GameObjectType::Player, id))
            .and_then(|object| object.to_player())
    }

    pub fn get_player_by_name(&self, name: &str) -> Option<objects::Player> {
        self.players_by_name
            .get(name)
            .and_then(|id| self.get_player(*id))
    }

    pub fn get_portal(&self, id: GameObjectId) -> Option<objects::Portal> {
        self.get(GameObjectRef(GameObjectType::Portal, id))
            .and_then(|object| object.to_portal())
    }

    pub fn get_race(&self, id: GameObjectId) -> Option<objects::Race> {
        self.get(GameObjectRef(GameObjectType::Race, id))
            .and_then(|object| object.to_race())
    }

    pub fn get_race_by_name(&self, name: &str) -> Option<objects::Race> {
        self.races_by_name
            .get(name)
            .and_then(|id| self.get_race(*id))
    }

    pub fn get_room(&self, id: GameObjectId) -> Option<objects::Room> {
        self.get(GameObjectRef(GameObjectType::Room, id))
            .and_then(|object| object.to_room())
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Arc<dyn GameObject>, String> {
        match serde_json::from_str::<RealmDto>(json) {
            Ok(realm_dto) => Ok(Arc::new(Self {
                date_time: realm_dto.dateTime,
                id,
                objects: HashMap::new(),
                players_by_name: HashMap::new(),
                name: realm_dto.name,
                races_by_name: HashMap::new(),
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn player_ids(&self) -> Vec<GameObjectId> {
        self.players_by_name.values().map(|id| *id).collect()
    }

    pub fn race_ids(&self) -> Vec<GameObjectId> {
        self.races_by_name.values().map(|id| *id).collect()
    }

    pub fn race_names(&self) -> Vec<String> {
        self.races_by_name.keys().map(|name| name.clone()).collect()
    }

    pub fn set(&self, object_ref: GameObjectRef, object: Arc<dyn GameObject>) -> Self {
        let mut objects = self.objects.clone();
        objects.insert(object_ref, object.clone());

        let mut players_by_name = self.players_by_name.clone();
        if let Some(player) = object.to_player() {
            players_by_name.insert(player.get_name(), player.get_id());
        }

        let mut races_by_name = self.races_by_name.clone();
        if let Some(race) = object.to_race() {
            races_by_name.insert(race.get_name(), race.get_id());
        }

        Self {
            name: self.name.clone(),
            objects,
            players_by_name,
            races_by_name,
            ..*self
        }
    }

    pub fn unset(&mut self, object_ref: GameObjectRef) -> Self {
        match self.objects.get(&object_ref) {
            Some(object) => {
                let mut objects = self.objects.clone();
                objects.remove(&object_ref);

                let name = object.get_name();

                let mut players_by_name = self.players_by_name.clone();
                players_by_name.remove(&name);

                let mut races_by_name = self.races_by_name.clone();
                races_by_name.remove(&name);

                Self {
                    name: self.name.clone(),
                    objects,
                    players_by_name,
                    races_by_name,
                    ..*self
                }
            }
            None => self.clone(),
        }
    }
}

impl GameObject for Realm {
    fn get_id(&self) -> GameObjectId {
        self.id
    }

    fn get_object_type(&self) -> GameObjectType {
        GameObjectType::Realm
    }

    fn get_name(&self) -> String {
        self.name.clone()
    }

    fn to_realm(&self) -> Option<Self> {
        Some(self.clone())
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct RealmDto {
    dateTime: u64,
    name: String,
}
