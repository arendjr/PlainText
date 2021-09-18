use crate::{game_object::GameObjectRef, objects::Realm};

pub trait Event {
    fn description_for_strength_and_character_in_room(
        &self,
        realm: &Realm,
        strength: f32,
        character: GameObjectRef,
        room: GameObjectRef,
    ) -> Option<String>;

    fn excluded_characters(&self) -> &[GameObjectRef];

    fn origins(&self) -> Vec<GameObjectRef>;
}
