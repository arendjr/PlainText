use crate::character_stats::CharacterStats;
use crate::game_object::{GameObject, GameObjectRef};
use crate::objects::Realm;
use crate::vector3d::Vector3D;

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum Gender {
    Unspecified,
    Male,
    Female,
}

impl Gender {
    pub fn from_str(gender_str: &str) -> Result<Self, String> {
        match gender_str {
            "male" => Ok(Gender::Male),
            "female" => Ok(Gender::Female),
            "unspecified" => Ok(Gender::Unspecified),
            _ => Err(format!("Unrecognized gender \"{}\"", gender_str)),
        }
    }

    pub fn hydrate(maybe_gender: &Option<String>) -> Self {
        match &maybe_gender.as_ref().map(String::as_str) {
            Some("male") => Gender::Male,
            Some("female") => Gender::Female,
            _ => Gender::Unspecified,
        }
    }

    pub fn serialize(&self) -> Option<String> {
        match self {
            Gender::Male => Some("male".to_owned()),
            Gender::Female => Some("female".to_owned()),
            Gender::Unspecified => None,
        }
    }
}

pub trait Character: GameObject {
    fn class(&self) -> Option<GameObjectRef>;
    fn set_class(&self, realm: Realm, class: Option<GameObjectRef>) -> Realm;

    fn current_room(&self) -> GameObjectRef;
    fn set_current_room(&self, realm: Realm, room: GameObjectRef) -> Realm;

    fn direction(&self) -> &Vector3D;
    fn set_direction(&self, realm: Realm, direction: Vector3D) -> Realm;

    fn gender(&self) -> Gender;
    fn set_gender(&self, realm: Realm, gender: Gender) -> Realm;

    fn gold(&self) -> u32;
    fn set_gold(&self, realm: Realm, gold: u32) -> Realm;

    fn height(&self) -> f32;
    fn set_height(&self, realm: Realm, height: f32) -> Realm;

    fn hp(&self) -> i16;
    fn set_hp(&self, realm: Realm, hp: i16) -> Realm;

    fn inventory(&self) -> &Vec<GameObjectRef>;
    fn set_inventory(&self, realm: Realm, inventory: Vec<GameObjectRef>) -> Realm;

    fn mp(&self) -> i16;
    fn set_mp(&self, realm: Realm, mp: i16) -> Realm;

    fn race(&self) -> GameObjectRef;
    fn set_race(&self, realm: Realm, race: GameObjectRef) -> Realm;

    fn stats(&self) -> &CharacterStats;
    fn set_stats(&self, realm: Realm, stats: CharacterStats) -> Realm;

    fn weight(&self) -> f32;
    fn set_weight(&self, realm: Realm, weight: f32) -> Realm;
}
