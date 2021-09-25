use lazy_static::__Deref;
use serde::{Deserialize, Serialize};
use std::cmp;
use std::collections::{HashMap, HashSet};
use std::hash::{Hash, Hasher};

use crate::game_object::{Character, GameObject, GameObjectId, GameObjectRef, GameObjectType};
use crate::objects;
use crate::objects::Player;
use crate::persistence_handler::PersistenceRequest;
use crate::sessions::SignUpData;

use super::Group;

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

pub struct Realm {
    date_time: u64,
    description: String,
    id: GameObjectId,
    name: String,
    needs_sync: bool,
    next_id: GameObjectId,
    objects: HashMap<GameObjectRef, Box<dyn GameObject>>,
    objects_to_be_removed: HashSet<GameObjectRef>,
    objects_to_be_synced: HashSet<GameObjectRef>,
    players_by_name: HashMap<String, GameObjectId>,
    races_by_name: HashMap<String, GameObjectId>,
}

impl Realm {
    game_object_copy_prop!(pub, date_time, set_date_time, u64);

    pub fn add_player(&mut self, sign_up_data: &SignUpData) {
        let player_ref = GameObjectRef(GameObjectType::Player, self.next_id);
        let mut player = Player::new(player_ref.id(), sign_up_data);
        if self.players_by_name.is_empty() {
            // First player automatically becomes admin:
            player.set_is_admin(true)
        }

        self.set(player_ref, Box::new(player));
    }

    pub fn character(&self, object_ref: GameObjectRef) -> Option<&dyn Character> {
        self.objects
            .get(&object_ref)
            .and_then(|object| object.as_character())
    }

    pub fn character_mut(&mut self, object_ref: GameObjectRef) -> Option<&mut dyn Character> {
        self.request_persistence(object_ref);
        self.objects
            .get_mut(&object_ref)
            .and_then(|object| object.as_character_mut())
    }

    pub fn character_res(&self, object_ref: GameObjectRef) -> Result<&dyn Character, &'static str> {
        self.character(object_ref)
            .ok_or("That character is no longer there.")
    }

    pub fn class(&self, object_ref: GameObjectRef) -> Option<&objects::Class> {
        self.objects
            .get(&object_ref)
            .and_then(|object| object.as_class())
    }

    pub fn create_group(&mut self, leader: GameObjectRef) -> GameObjectRef {
        let group_ref = GameObjectRef(GameObjectType::Group, self.next_id);
        let group = Group::new(group_ref.id(), leader);
        if let Some(leader) = self.character_mut(leader) {
            leader.set_group(group_ref);
        }

        self.set(group_ref, Box::new(group));

        group_ref
    }

    pub fn group(&self, object_ref: GameObjectRef) -> Option<&objects::Group> {
        self.objects
            .get(&object_ref)
            .and_then(|object| object.as_group())
    }

    pub fn group_mut(&mut self, object_ref: GameObjectRef) -> Option<&mut objects::Group> {
        self.objects
            .get_mut(&object_ref)
            .and_then(|object| object.as_group_mut())
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Realm, String> {
        match serde_json::from_str::<RealmDto>(json) {
            Ok(realm_dto) => Ok(Self {
                date_time: realm_dto.dateTime,
                description: realm_dto.description.unwrap_or_default(),
                id,
                objects: HashMap::new(),
                objects_to_be_removed: HashSet::new(),
                objects_to_be_synced: HashSet::new(),
                players_by_name: HashMap::new(),
                name: realm_dto.name,
                needs_sync: false,
                next_id: id + 1,
                races_by_name: HashMap::new(),
            }),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn item(&self, object_ref: GameObjectRef) -> Option<&objects::Item> {
        self.objects
            .get(&object_ref)
            .and_then(|object| object.as_item())
    }

    pub fn next_id(&mut self) -> GameObjectId {
        let id = self.next_id;
        self.next_id += 1;
        id
    }

    pub fn object(&self, object_ref: GameObjectRef) -> Option<&dyn GameObject> {
        self.objects.get(&object_ref).map(|object| object.deref())
    }

    pub fn object_mut(&mut self, object_ref: GameObjectRef) -> Option<&mut dyn GameObject> {
        self.request_persistence(object_ref);
        self.objects
            .get_mut(&object_ref)
            .and_then(|object| object.as_object_mut())
    }

    pub fn object_res(&self, object_ref: GameObjectRef) -> Result<&dyn GameObject, &'static str> {
        self.objects
            .get(&object_ref)
            .map(|object| object.deref())
            .ok_or("Unknown object.")
    }

    pub fn objects_of_type(
        &self,
        object_type: GameObjectType,
    ) -> impl Iterator<Item = &dyn GameObject> + '_ {
        self.objects
            .iter()
            .filter(move |(object_ref, _)| object_ref.object_type() == object_type)
            .map(|(_, object)| object.deref())
    }

    pub fn player(&self, object_ref: GameObjectRef) -> Option<&objects::Player> {
        self.objects
            .get(&object_ref)
            .and_then(|object| object.as_player())
    }

    pub fn player_mut(&mut self, object_ref: GameObjectRef) -> Option<&mut objects::Player> {
        self.request_persistence(object_ref);
        self.objects
            .get_mut(&object_ref)
            .and_then(|object| object.as_player_mut())
    }

    pub fn player_by_id(&self, id: GameObjectId) -> Option<&objects::Player> {
        self.player(GameObjectRef(GameObjectType::Player, id))
    }

    pub fn player_by_id_mut(&mut self, id: GameObjectId) -> Option<&mut objects::Player> {
        self.player_mut(GameObjectRef(GameObjectType::Player, id))
    }

    pub fn player_by_name(&self, name: &str) -> Option<&objects::Player> {
        self.players_by_name
            .get(name)
            .and_then(|id| self.player_by_id(*id))
    }

    pub fn player_by_name_mut(&mut self, name: &str) -> Option<&mut objects::Player> {
        self.players_by_name
            .get(name)
            .copied()
            .and_then(move |id| self.player_by_id_mut(id))
    }

    pub fn player_res(&self, object_ref: GameObjectRef) -> Result<&objects::Player, &'static str> {
        self.player(object_ref)
            .ok_or("Your account has been deactivated.")
    }

    pub fn player_and_room_res(
        &self,
        object_ref: GameObjectRef,
    ) -> Result<(&objects::Player, &objects::Room), &'static str> {
        let player = self.player_res(object_ref)?;
        let room = self
            .room(player.current_room())
            .ok_or("You have slipped between dimensions.")?;
        Ok((player, room))
    }

    pub fn portal(&self, object_ref: GameObjectRef) -> Option<&objects::Portal> {
        self.objects
            .get(&object_ref)
            .and_then(|object| object.as_portal())
    }

    pub fn race(&self, object_ref: GameObjectRef) -> Option<&objects::Race> {
        self.objects
            .get(&object_ref)
            .and_then(|object| object.as_race())
    }

    pub fn race_by_name(&self, name: &str) -> Option<&objects::Race> {
        self.races_by_name
            .get(name)
            .and_then(|id| self.race(GameObjectRef(GameObjectType::Race, *id)))
    }

    pub fn race_names(&self) -> Vec<&str> {
        self.races_by_name.keys().map(String::as_ref).collect()
    }

    pub fn request_persistence(&mut self, object_ref: GameObjectRef) {
        self.objects_to_be_synced.insert(object_ref);
    }

    pub fn room(&self, object_ref: GameObjectRef) -> Option<&objects::Room> {
        self.objects
            .get(&object_ref)
            .and_then(|object| object.as_room())
    }

    pub fn room_mut(&mut self, object_ref: GameObjectRef) -> Option<&mut objects::Room> {
        self.request_persistence(object_ref);
        self.objects
            .get_mut(&object_ref)
            .and_then(|object| object.as_room_mut())
    }

    pub fn room_res(&self, object_ref: GameObjectRef) -> Result<&objects::Room, &'static str> {
        self.room(object_ref)
            .ok_or("The room was sucked into the void.")
    }

    pub fn set(&mut self, object_ref: GameObjectRef, object: Box<dyn GameObject>) {
        if let Some(player) = object.as_player() {
            self.players_by_name
                .insert(player.name().to_owned(), player.id());
        }

        if let Some(race) = object.as_race() {
            self.races_by_name.insert(race.name().to_owned(), race.id());
        }

        if object.needs_sync() {
            self.request_persistence(object_ref);
        }

        self.objects.insert(object_ref, object);

        self.next_id = cmp::max(self.next_id, object_ref.id() + 1);
    }

    pub fn take_persistence_requests(&mut self) -> Vec<PersistenceRequest> {
        let mut result = vec![];
        for object_ref in self.objects_to_be_synced.clone().drain() {
            if let Some(object) = self.object_mut(object_ref) {
                if object.needs_sync() {
                    result.push(PersistenceRequest::PersistObject(
                        object_ref,
                        object.dehydrate().to_string(),
                    ));
                    object.set_needs_sync(false);
                }
            }
        }
        for object_ref in self.objects_to_be_removed.drain() {
            result.push(PersistenceRequest::RemoveObject(object_ref))
        }
        result
    }

    pub fn unset(&mut self, object_ref: GameObjectRef) {
        if let Some(object) = self.objects.get(&object_ref) {
            let name = object.name();
            if object.as_player().is_some() {
                self.players_by_name.remove(name);
            } else if object.as_race().is_some() {
                self.races_by_name.remove(name);
            }

            self.objects.remove(&object_ref);

            self.objects_to_be_removed.insert(object_ref);
            self.objects_to_be_synced.remove(&object_ref);
        }
    }
}

impl GameObject for Realm {
    game_object_string_prop!(name, set_name);
    game_object_string_prop!(description, set_description);

    fn as_object(&self) -> Option<&dyn GameObject> {
        Some(self)
    }

    fn as_object_mut(&mut self) -> Option<&mut dyn GameObject> {
        Some(self)
    }

    fn as_realm(&self) -> Option<&Self> {
        Some(self)
    }

    fn dehydrate(&self) -> serde_json::Value {
        serde_json::to_value(RealmDto {
            dateTime: self.date_time,
            description: if self.description.is_empty() {
                None
            } else {
                Some(self.description.clone())
            },
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

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn needs_sync(&self) -> bool {
        self.needs_sync
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Realm
    }

    fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "date_time" => {
                self.set_date_time(value.parse().map_err(|error| format!("{:?}", error))?)
            }
            "description" => self.set_description(value.to_owned()),
            "name" => self.set_name(value.to_owned()),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct RealmDto {
    dateTime: u64,
    description: Option<String>,
    name: String,
}
