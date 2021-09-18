use crate::{
    game_object::GameObjectRef,
    objects::{Realm, Room},
};

pub trait Event {
    fn description_for_strength_and_character_in_room(
        &self,
        realm: &Realm,
        strength: f32,
        character: GameObjectRef,
        room: GameObjectRef,
    ) -> Option<String>;

    fn excluded_characters(&self) -> &[GameObjectRef];

    fn origin(&self) -> GameObjectRef;
}

pub trait VisualEvent: Event {
    fn is_within_sight(&self, realm: &Realm, target_room: &Room, source_room: &Room) -> bool;
}
