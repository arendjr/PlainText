use crate::{
    entity::{Entity, EntityId, EntityRef, EntityType},
    player_output::PlayerOutput,
    sessions::SessionOutput,
};
use serde::Serialize;

#[derive(Clone, Debug, PartialEq, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Group {
    id: EntityId,
    leader: EntityRef,
    followers: Vec<EntityRef>,
}

impl Group {
    pub fn new(id: EntityId, leader: EntityRef) -> Self {
        Self {
            id,
            leader,
            followers: vec![],
        }
    }

    pub fn leader(&self) -> EntityRef {
        self.leader
    }

    pub fn set_leader(&mut self, leader: EntityRef) {
        self.leader = leader;
    }

    pub fn followers(&self) -> &[EntityRef] {
        &self.followers
    }

    pub fn add_follower(&mut self, follower: EntityRef) {
        self.followers.push(follower);
    }

    pub fn remove_follower(&mut self, follower: EntityRef) {
        self.followers.retain(|existing| *existing != follower);
    }

    pub fn set_followers(&mut self, followers: Vec<EntityRef>) {
        self.followers = followers;
    }

    pub fn send_all(&self, output: SessionOutput) -> Vec<PlayerOutput> {
        let mut all_output = vec![PlayerOutput::new(self.leader.id(), output.clone())];
        for follower in self.followers.iter() {
            all_output.push(PlayerOutput::new(follower.id(), output.clone()));
        }
        all_output
    }
}

impl Entity for Group {
    fn name(&self) -> &str {
        "some people"
    }

    fn set_name(&mut self, _: String) {}

    fn description(&self) -> &str {
        ""
    }

    fn set_description(&mut self, _: String) {}

    fn adjective(&self) -> &str {
        ""
    }

    fn as_group(&self) -> Option<&Self> {
        Some(self)
    }

    fn as_group_mut(&mut self) -> Option<&mut Self> {
        Some(self)
    }

    fn dehydrate(&self) -> String {
        panic!("Groups don't support serialization")
    }

    fn entity_ref(&self) -> EntityRef {
        EntityRef::new(EntityType::Group, self.id)
    }

    fn id(&self) -> EntityId {
        self.id
    }

    fn needs_sync(&self) -> bool {
        false
    }

    fn set_needs_sync(&mut self, _: bool) {}

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "leader" => self.set_leader(EntityRef::from_str(value)?),
            "followers" => self.set_followers(EntityRef::vec_from_str(value)?),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }

    fn to_json_value(&self) -> serde_json::Result<serde_json::Value> {
        serde_json::to_value(self)
    }
}
