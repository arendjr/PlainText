use super::Realm;
use crate::{
    game_object::{GameObject, GameObjectId, GameObjectRef, GameObjectType, Gender},
    player_output::PlayerOutput,
    sessions::SessionOutput,
    text_utils::join_sentence,
};
use std::fmt;

#[derive(Clone, Debug, PartialEq)]
pub struct Group {
    id: GameObjectId,
    leader: GameObjectRef,
    followers: Vec<GameObjectRef>,
}

impl Group {
    pub fn new(id: GameObjectId, leader: GameObjectRef) -> Self {
        Self {
            id,
            leader,
            followers: vec![],
        }
    }

    fn fallible_name_at_strength(&self, realm: &Realm, strength: f32) -> Result<String, String> {
        let leader = realm.character_res(self.leader)?;
        let room = realm.room_res(leader.current_room())?;

        let mut party = vec![leader.object_ref()];
        for follower in self.followers.iter() {
            let is_in_same_room = realm
                .character(*follower)
                .map(|follower| follower.current_room() == room.object_ref())
                .unwrap_or(false);
            if is_in_same_room {
                party.push(*follower);
            }
        }

        Ok(if strength > 0.9 {
            join_sentence(
                &party
                    .iter()
                    .filter_map(|character| realm.character(*character)?.definite_name(realm).ok())
                    .collect::<Vec<_>>(),
            )
        } else if strength > 0.8 {
            let mut num_males = 0;
            let mut num_females = 0;
            let mut num_others = 0;
            for character in party.iter() {
                let character = realm.character_res(*character)?;
                match character.gender() {
                    Gender::Male => num_males += 1,
                    Gender::Female => num_females += 1,
                    Gender::Unspecified => num_others += 1,
                }
            }
            let what = if num_others > 0 {
                "people"
            } else if num_males > 0 {
                if num_females > 0 {
                    "people"
                } else {
                    "men"
                }
            } else {
                "women"
            };
            if party.len() > 8 {
                format!("a lot of {}", what)
            } else if party.len() > 2 {
                format!("a group of {}", what)
            } else {
                format!("two {}", what)
            }
        } else if party.len() > 8 {
            "a lot of people".to_owned()
        } else {
            "some people".to_owned()
        })
    }

    pub fn leader(&self) -> GameObjectRef {
        self.leader
    }

    pub fn set_leader(&mut self, leader: GameObjectRef) {
        self.leader = leader;
    }

    pub fn followers(&self) -> &[GameObjectRef] {
        &self.followers
    }

    pub fn add_follower(&mut self, follower: GameObjectRef) {
        self.followers.push(follower);
    }

    pub fn remove_follower(&mut self, follower: GameObjectRef) {
        self.followers.retain(|existing| *existing != follower);
    }

    pub fn set_followers(&mut self, followers: Vec<GameObjectRef>) {
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

impl fmt::Display for Group {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Group({})", self.id)
    }
}

impl GameObject for Group {
    fn name(&self) -> &str {
        "some people"
    }

    fn name_at_strength(&self, realm: &Realm, strength: f32) -> String {
        self.fallible_name_at_strength(realm, strength)
            .unwrap_or_else(|_| self.name().into())
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

    fn as_object(&self) -> Option<&dyn GameObject> {
        Some(self)
    }

    fn as_object_mut(&mut self) -> Option<&mut dyn GameObject> {
        Some(self)
    }

    fn dehydrate(&self) -> serde_json::Value {
        panic!("Groups don't support serialization")
    }

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn needs_sync(&self) -> bool {
        false
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Group
    }

    fn set_needs_sync(&mut self, _: bool) {}

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "leader" => self.set_leader(GameObjectRef::from_str(value)?),
            "followers" => self.set_followers(GameObjectRef::vec_from_str(value)?),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }
}
