use crate::character_stats::CharacterStats;
use crate::game_object::{GameObject, GameObjectRef, SharedGameObject};
use crate::vector3d::Vector3D;

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum Gender {
    Unspecified,
    Male,
    Female,
}

impl Gender {
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
    fn current_room(&self) -> GameObjectRef;
    fn direction(&self) -> &Vector3D;
    fn gender(&self) -> Gender;
    fn gold(&self) -> u32;
    fn height(&self) -> f32;
    fn hp(&self) -> i16;
    fn max_hp(&self) -> i16;
    fn max_mp(&self) -> i16;
    fn mp(&self) -> i16;
    fn race(&self) -> GameObjectRef;
    fn stats(&self) -> &CharacterStats;
    fn weight(&self) -> f32;
    fn with_current_room(&self, room: GameObjectRef) -> (SharedGameObject, bool);
}
