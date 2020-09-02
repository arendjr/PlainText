use serde::{Deserialize, Serialize};
use serde_json;
use std::cmp;
use std::collections::HashSet;
use std::hash::{Hash, Hasher};

use im_rc::HashMap;

use crate::objects;

use crate::game_object::{
    GameObject, GameObjectId, GameObjectRef, GameObjectType, SharedGameObject, SharedObject,
};
use crate::objects::Player;
use crate::persistence_thread::PersistenceRequest;
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
    objects: HashMap<GameObjectRef, SharedGameObject>,
    objects_to_be_removed: HashSet<GameObjectRef>,
    objects_to_be_synced: HashSet<GameObjectRef>,
    players_by_name: HashMap<String, GameObjectId>,
    races_by_name: HashMap<String, GameObjectId>,
}

impl Realm {
    pub fn class(&self, id: GameObjectId) -> Option<SharedObject<objects::Class>> {
        self.object(GameObjectRef(GameObjectType::Class, id))
            .and_then(|object| SharedObject::new(object, |object| object.as_class()))
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<SharedGameObject, String> {
        match serde_json::from_str::<RealmDto>(json) {
            Ok(realm_dto) => Ok(SharedGameObject::new(Self {
                date_time: realm_dto.dateTime,
                id,
                objects: HashMap::new(),
                objects_to_be_removed: HashSet::new(),
                objects_to_be_synced: HashSet::new(),
                players_by_name: HashMap::new(),
                name: realm_dto.name,
                next_id: id + 1,
                races_by_name: HashMap::new(),
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn item(&self, id: GameObjectId) -> Option<SharedObject<objects::Item>> {
        self.object(GameObjectRef(GameObjectType::Item, id))
            .and_then(|object| SharedObject::new(object, |object| object.as_item()))
    }

    pub fn object(&self, object_ref: GameObjectRef) -> Option<SharedGameObject> {
        self.objects.get(&object_ref).map(|object| object.clone())
    }

    pub fn player(&self, id: GameObjectId) -> Option<SharedObject<objects::Player>> {
        self.object(GameObjectRef(GameObjectType::Player, id))
            .and_then(|object| SharedObject::new(object, |object| object.as_player()))
    }

    pub fn player_by_name(&self, name: &str) -> Option<SharedObject<objects::Player>> {
        self.players_by_name
            .get(name)
            .and_then(|id| self.player(*id))
    }

    pub fn player_ids(&self) -> Vec<GameObjectId> {
        self.players_by_name.values().map(|id| *id).collect()
    }

    pub fn portal(&self, id: GameObjectId) -> Option<SharedObject<objects::Portal>> {
        self.object(GameObjectRef(GameObjectType::Portal, id))
            .and_then(|object| SharedObject::new(object, |object| object.as_portal()))
    }

    pub fn race(&self, id: GameObjectId) -> Option<SharedObject<objects::Race>> {
        self.object(GameObjectRef(GameObjectType::Race, id))
            .and_then(|object| SharedObject::new(object, |object| object.as_race()))
    }

    pub fn race_by_name(&self, name: &str) -> Option<SharedObject<objects::Race>> {
        self.races_by_name.get(name).and_then(|id| self.race(*id))
    }

    pub fn race_names(&self) -> Vec<&str> {
        self.races_by_name.keys().map(String::as_ref).collect()
    }

    pub fn room(&self, id: GameObjectId) -> Option<SharedObject<objects::Room>> {
        self.object(GameObjectRef(GameObjectType::Room, id))
            .and_then(|object| SharedObject::new(object, |object| object.as_room()))
    }

    pub fn set<T: GameObject>(
        &self,
        object_ref: GameObjectRef,
        (object, should_sync): (T, bool),
    ) -> Self
    where
        T: 'static,
    {
        self.set_shared_object(object_ref, (SharedGameObject::new(object), should_sync))
    }

    pub fn set_shared_object(
        &self,
        object_ref: GameObjectRef,
        (object, should_sync): (SharedGameObject, bool),
    ) -> Self {
        let mut players_by_name = self.players_by_name.clone();
        if let Some(player) = object.as_player() {
            players_by_name.insert(player.name().to_owned(), player.id());
        }

        let mut races_by_name = self.races_by_name.clone();
        if let Some(race) = object.as_race() {
            races_by_name.insert(race.name().to_owned(), race.id());
        }

        let mut objects = self.objects.clone();
        objects.insert(object_ref, object);

        Self {
            name: self.name.clone(),
            next_id: cmp::max(self.next_id, object_ref.id() + 1),
            objects,
            objects_to_be_removed: self.objects_to_be_removed.clone(),
            objects_to_be_synced: if should_sync {
                let mut new_objects = self.objects_to_be_synced.clone();
                new_objects.insert(object_ref);
                new_objects
            } else {
                self.objects_to_be_synced.clone()
            },
            players_by_name,
            races_by_name,
            ..*self
        }
    }

    pub fn take_persistence_requests(&mut self) -> Vec<PersistenceRequest> {
        let mut result = vec![];
        for object_ref in self.objects_to_be_synced.clone().drain() {
            if let Some(object) = self.object(object_ref) {
                result.push(PersistenceRequest::PersistObject(
                    object_ref,
                    object.serialize(),
                ));
            }
        }
        for object_ref in self.objects_to_be_removed.drain() {
            result.push(PersistenceRequest::RemoveObject(object_ref))
        }
        result
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
                    objects_to_be_removed: {
                        let mut new_objects = self.objects_to_be_removed.clone();
                        new_objects.insert(object_ref);
                        new_objects
                    },
                    objects_to_be_synced: {
                        let mut new_objects = self.objects_to_be_synced.clone();
                        new_objects.remove(&object_ref);
                        new_objects
                    },
                    players_by_name,
                    races_by_name,
                    ..*self
                }
            }
            None => self.clone(),
        }
    }

    pub fn with_new_player(&self, sign_up_data: &SignUpData) -> Self {
        let player_ref = GameObjectRef(GameObjectType::Player, self.next_id);
        self.set(player_ref, Player::new(player_ref.id(), sign_up_data))
    }
}

impl GameObject for Realm {
    fn as_realm(&self) -> Option<&Self> {
        Some(&self)
    }

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Realm
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn serialize(&self) -> String {
        serde_json::to_string_pretty(&RealmDto {
            dateTime: self.date_time,
            name: self.name.clone(),
        })
        .unwrap_or_else(|error| {
            panic!(
                "Failed to serialize object {:?}: {:?}",
                self.object_ref(),
                error
            )
        })
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct RealmDto {
    dateTime: u64,
    name: String,
}
