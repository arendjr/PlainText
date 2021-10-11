use crate::entity::{EntityRef, Realm};

pub trait Event {
    fn description_for_strength_and_character_in_room(
        &self,
        realm: &Realm,
        strength: f32,
        character: EntityRef,
        room: EntityRef,
    ) -> Option<String>;

    fn excluded_characters(&self) -> &[EntityRef];

    fn origins(&self) -> Vec<EntityRef>;
}
