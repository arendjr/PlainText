use crate::character_stats::CharacterStats;
use crate::game_object::{GameObject, GameObjectRef};
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
    fn set_class(&mut self, class: Option<GameObjectRef>);

    fn current_room(&self) -> GameObjectRef;
    fn set_current_room(&mut self, room: GameObjectRef);

    fn direction(&self) -> &Vector3D;
    fn set_direction(&mut self, direction: Vector3D);

    fn gender(&self) -> Gender;
    fn set_gender(&mut self, gender: Gender);

    fn gold(&self) -> u32;
    fn set_gold(&mut self, gold: u32);

    fn height(&self) -> f32;
    fn set_height(&mut self, height: f32);

    fn hp(&self) -> i16;
    fn set_hp(&mut self, hp: i16);

    fn inventory(&self) -> &Vec<GameObjectRef>;
    fn set_inventory(&mut self, inventory: Vec<GameObjectRef>);

    fn mp(&self) -> i16;
    fn set_mp(&mut self, mp: i16);

    fn race(&self) -> GameObjectRef;
    fn set_race(&mut self, race: GameObjectRef);

    fn stats(&self) -> &CharacterStats;
    fn set_stats(&mut self, stats: CharacterStats);

    fn weight(&self) -> f32;
    fn set_weight(&mut self, weight: f32);
}
