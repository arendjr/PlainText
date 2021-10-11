use super::Group;
use crate::{
    entity::*, entity_copy_prop, entity_string_prop, persistence_handler::PersistenceRequest,
    sessions::SignUpData,
};
use core::panic;
use lazy_static::__Deref;
use serde::{Deserialize, Serialize};
use std::{
    cmp,
    collections::{HashMap, HashSet},
    hash::{Hash, Hasher},
};

impl Eq for dyn Entity {}

impl Hash for dyn Entity {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.id().hash(state);
        self.entity_type().hash(state);
    }
}

impl PartialEq for dyn Entity {
    fn eq(&self, other: &Self) -> bool {
        self.id() == other.id()
    }
}

#[derive(Deserialize, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Realm {
    date_time: u64,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    description: String,
    #[serde(skip)]
    entities: HashMap<EntityRef, Box<dyn Entity>>,
    #[serde(skip)]
    entities_to_be_removed: HashSet<EntityRef>,
    #[serde(skip)]
    entities_to_be_synced: HashSet<EntityRef>,
    #[serde(skip)]
    id: EntityId,
    name: String,
    #[serde(skip)]
    needs_sync: bool,
    #[serde(skip)]
    next_id: EntityId,
    #[serde(skip)]
    players_by_name: HashMap<String, EntityId>,
    #[serde(skip)]
    races_by_name: HashMap<String, EntityId>,
}

impl Realm {
    entity_copy_prop!(pub, date_time, set_date_time, u64);

    pub fn add_player(&mut self, sign_up_data: &SignUpData) {
        let player_ref = EntityRef(EntityType::Player, self.next_id);
        let mut player = Player::new(player_ref.id(), sign_up_data);
        if self.players_by_name.is_empty() {
            // First player automatically becomes admin:
            player.set_is_admin(true)
        }

        self.set(player_ref, Box::new(player));
    }

    pub fn character(&self, entity_ref: EntityRef) -> Option<&Character> {
        self.entities
            .get(&entity_ref)
            .and_then(|entity| entity.as_character())
    }

    pub fn character_mut(&mut self, entity_ref: EntityRef) -> Option<&mut Character> {
        self.request_persistence(entity_ref);
        self.entities
            .get_mut(&entity_ref)
            .and_then(|entity| entity.as_character_mut())
    }

    pub fn character_res(&self, entity_ref: EntityRef) -> Result<&Character, &'static str> {
        self.character(entity_ref)
            .ok_or("That character is no longer there.")
    }

    pub fn character_and_room_res(
        &self,
        entity_ref: EntityRef,
    ) -> Result<(&Character, &Room), &'static str> {
        let character = self.character_res(entity_ref)?;
        let room = self
            .room(character.current_room())
            .ok_or("You have slipped between dimensions.")?;
        Ok((character, room))
    }

    pub fn class(&self, entity_ref: EntityRef) -> Option<&Class> {
        self.entities
            .get(&entity_ref)
            .and_then(|entity| entity.as_class())
    }

    pub fn create_group(&mut self, leader: EntityRef) -> EntityRef {
        let group_ref = EntityRef(EntityType::Group, self.next_id);
        let group = Group::new(group_ref.id(), leader);
        if let Some(leader) = self.character_mut(leader) {
            leader.set_group(group_ref);
        }

        self.set(group_ref, Box::new(group));

        group_ref
    }

    pub fn group(&self, entity_ref: EntityRef) -> Option<&entities::Group> {
        self.entities
            .get(&entity_ref)
            .and_then(|entity| entity.as_group())
    }

    pub fn group_mut(&mut self, entity_ref: EntityRef) -> Option<&mut entities::Group> {
        self.request_persistence(entity_ref);
        self.entities
            .get_mut(&entity_ref)
            .and_then(|entity| entity.as_group_mut())
    }

    pub fn hydrate(id: EntityId, json: &str) -> Result<Realm, String> {
        let mut realm = serde_json::from_str::<Realm>(json)
            .map_err(|error| format!("parse error: {}", error))?;
        realm.id = id;
        realm.next_id = id + 1;
        Ok(realm)
    }

    pub fn item(&self, entity_ref: EntityRef) -> Option<&entities::Item> {
        self.entities
            .get(&entity_ref)
            .and_then(|entity| entity.as_item())
    }

    pub fn next_id(&mut self) -> EntityId {
        let id = self.next_id;
        self.next_id += 1;
        id
    }

    pub fn entity(&self, entity_ref: EntityRef) -> Option<&dyn Entity> {
        self.entities.get(&entity_ref).map(|entity| entity.deref())
    }

    pub fn entity_mut(&mut self, entity_ref: EntityRef) -> Option<&mut dyn Entity> {
        self.request_persistence(entity_ref);
        self.entities
            .get_mut(&entity_ref)
            .and_then(|entity| entity.as_entity_mut())
    }

    pub fn entity_res(&self, entity_ref: EntityRef) -> Result<&dyn Entity, &'static str> {
        self.entities
            .get(&entity_ref)
            .map(|entity| entity.deref())
            .ok_or("Unknown entity.")
    }

    pub fn entities_of_type(
        &self,
        entity_type: EntityType,
    ) -> impl Iterator<Item = &dyn Entity> + '_ {
        self.entities
            .iter()
            .filter(move |(entity_ref, _)| entity_ref.entity_type() == entity_type)
            .map(|(_, entity)| entity.deref())
    }

    pub fn player(&self, entity_ref: EntityRef) -> Option<&entities::Player> {
        self.entities
            .get(&entity_ref)
            .and_then(|entity| entity.as_player())
    }

    pub fn player_mut(&mut self, entity_ref: EntityRef) -> Option<&mut entities::Player> {
        self.request_persistence(entity_ref);
        self.entities
            .get_mut(&entity_ref)
            .and_then(|entity| entity.as_player_mut())
    }

    pub fn player_by_id(&self, id: EntityId) -> Option<&entities::Player> {
        self.player(EntityRef(EntityType::Player, id))
    }

    pub fn player_by_id_mut(&mut self, id: EntityId) -> Option<&mut entities::Player> {
        self.player_mut(EntityRef(EntityType::Player, id))
    }

    pub fn player_by_name(&self, name: &str) -> Option<&entities::Player> {
        self.players_by_name
            .get(name)
            .and_then(|id| self.player_by_id(*id))
    }

    pub fn player_by_name_mut(&mut self, name: &str) -> Option<&mut entities::Player> {
        self.players_by_name
            .get(name)
            .copied()
            .and_then(move |id| self.player_by_id_mut(id))
    }

    pub fn player_res(&self, entity_ref: EntityRef) -> Result<&entities::Player, &'static str> {
        self.player(entity_ref)
            .ok_or("Your account has been deactivated.")
    }

    pub fn portal(&self, entity_ref: EntityRef) -> Option<&entities::Portal> {
        self.entities
            .get(&entity_ref)
            .and_then(|entity| entity.as_portal())
    }

    pub fn portal_mut(&mut self, entity_ref: EntityRef) -> Option<&mut entities::Portal> {
        self.request_persistence(entity_ref);
        self.entities
            .get_mut(&entity_ref)
            .and_then(|entity| entity.as_portal_mut())
    }

    pub fn race(&self, entity_ref: EntityRef) -> Option<&entities::Race> {
        self.entities
            .get(&entity_ref)
            .and_then(|entity| entity.as_race())
    }

    pub fn race_by_name(&self, name: &str) -> Option<&entities::Race> {
        self.races_by_name
            .get(name)
            .and_then(|id| self.race(EntityRef(EntityType::Race, *id)))
    }

    pub fn race_names(&self) -> Vec<&str> {
        self.races_by_name.keys().map(String::as_ref).collect()
    }

    pub fn request_persistence(&mut self, entity_ref: EntityRef) {
        self.entities_to_be_synced.insert(entity_ref);
    }

    pub fn room(&self, entity_ref: EntityRef) -> Option<&entities::Room> {
        self.entities
            .get(&entity_ref)
            .and_then(|entity| entity.as_room())
    }

    pub fn room_mut(&mut self, entity_ref: EntityRef) -> Option<&mut entities::Room> {
        self.request_persistence(entity_ref);
        self.entities
            .get_mut(&entity_ref)
            .and_then(|entity| entity.as_room_mut())
    }

    pub fn set(&mut self, entity_ref: EntityRef, entity: Box<dyn Entity>) {
        if let Some(player) = entity.as_player() {
            self.players_by_name
                .insert(player.name().to_owned(), player.id());
        }

        if let Some(race) = entity.as_race() {
            self.races_by_name.insert(race.name().to_owned(), race.id());
        }

        if entity.needs_sync() {
            self.request_persistence(entity_ref);
        }

        self.entities.insert(entity_ref, entity);

        self.next_id = cmp::max(self.next_id, entity_ref.id() + 1);
    }

    pub fn take_persistence_requests(&mut self) -> Vec<PersistenceRequest> {
        let mut result = vec![];
        for entity_ref in self.entities_to_be_synced.clone().drain() {
            if let Some(entity) = self.entity_mut(entity_ref) {
                if entity.needs_sync() {
                    result.push(PersistenceRequest::PersistEntity(
                        entity_ref,
                        entity.dehydrate(),
                    ));
                    entity.set_needs_sync(false);
                }
            }
        }
        for entity_ref in self.entities_to_be_removed.drain() {
            result.push(PersistenceRequest::RemoveEntity(entity_ref))
        }
        result
    }

    pub fn unset(&mut self, entity_ref: EntityRef) {
        if let Some(entity) = self.entities.get(&entity_ref) {
            let name = entity.name();
            if entity.as_player().is_some() {
                self.players_by_name.remove(name);
            } else if entity.as_race().is_some() {
                self.races_by_name.remove(name);
            }

            self.entities.remove(&entity_ref);

            self.entities_to_be_removed.insert(entity_ref);
            self.entities_to_be_synced.remove(&entity_ref);
        }
    }
}

impl Entity for Realm {
    entity_string_prop!(name, set_name);
    entity_string_prop!(description, set_description);

    fn as_entity(&self) -> Option<&dyn Entity> {
        Some(self)
    }

    fn as_entity_mut(&mut self) -> Option<&mut dyn Entity> {
        Some(self)
    }

    fn as_realm(&self) -> Option<&Self> {
        Some(self)
    }

    fn dehydrate(&self) -> String {
        serde_json::to_string_pretty(self).unwrap_or_else(|error| {
            panic!(
                "Failed to serialize entity {:?}: {:?}",
                self.entity_ref(),
                error
            )
        })
    }

    fn entity_ref(&self) -> EntityRef {
        EntityRef::new(EntityType::Realm, self.id)
    }

    fn id(&self) -> EntityId {
        self.id
    }

    fn needs_sync(&self) -> bool {
        self.needs_sync
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

    fn to_json_value(&self) -> serde_json::Result<serde_json::Value> {
        serde_json::to_value(self)
    }
}
