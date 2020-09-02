use serde::{Deserialize, Serialize};
use serde_json;
use std::cmp;
use std::hash::{Hash, Hasher};
use std::sync::Arc;

use im_rc::HashMap;

use crate::objects;

use crate::game_object::{GameObject, GameObjectId, GameObjectRef, GameObjectType};
use crate::objects::Player;
use crate::sessions::SignUpData;

impl Eq for dyn GameObject {}

impl Hash for dyn GameObject {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.id().hash(state);
        self.object_type().hash(state);
    }
}

impl PartialEq for dyn GameObject {
    fn eq(&self, other: &Self) -> bool {
        self.id() == other.id()
    }
}

#[derive(Clone)]
pub struct Realm {
    date_time: u64,
    id: GameObjectId,
    name: String,
    next_id: GameObjectId,
    objects: HashMap<GameObjectRef, Arc<dyn GameObject>>,
    players_by_name: HashMap<String, GameObjectId>,
    races_by_name: HashMap<String, GameObjectId>,
}

impl Realm {
    pub fn create_player(&self, sign_up_data: &SignUpData) -> Self {
        let player_ref = GameObjectRef(GameObjectType::Player, self.next_id);
        let player = Player::new(player_ref.id(), sign_up_data);
        self.set(player_ref, Arc::new(player))
    }

    pub fn get(&self, object_ref: GameObjectRef) -> Option<Arc<dyn GameObject>> {
        self.objects.get(&object_ref).map(|object| object.clone())
    }

    pub fn get_class(&self, id: GameObjectId) -> Option<objects::Class> {
        self.get(GameObjectRef(GameObjectType::Class, id))
            .and_then(|object| object.to_class())
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
                next_id: id + 1,
                races_by_name: HashMap::new(),
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn player_ids(&self) -> Vec<GameObjectId> {
        self.players_by_name.values().map(|id| *id).collect()
    }

    pub fn race_names(&self) -> Vec<&str> {
        self.races_by_name.keys().map(String::as_ref).collect()
    }

    pub fn set(&self, object_ref: GameObjectRef, object: Arc<dyn GameObject>) -> Self {
        let mut objects = self.objects.clone();
        objects.insert(object_ref, object.clone());

        let mut players_by_name = self.players_by_name.clone();
        if let Some(player) = object.to_player() {
            players_by_name.insert(player.name().to_owned(), player.id());
        }

        let mut races_by_name = self.races_by_name.clone();
        if let Some(race) = object.to_race() {
            races_by_name.insert(race.name().to_owned(), race.id());
        }

        Self {
            name: self.name.clone(),
            next_id: cmp::max(self.next_id, object_ref.id() + 1),
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

                let name = object.name();

                let mut players_by_name = self.players_by_name.clone();
                players_by_name.remove(name);

                let mut races_by_name = self.races_by_name.clone();
                races_by_name.remove(name);

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
    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Realm
    }

    fn name(&self) -> &str {
        &self.name
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
