use crate::{game_object::GameObjectRef, objects::Realm, player_output::PlayerOutput};

use super::{event::Event, visible_room_visitor};

/// A generic, visual event that can get triggered for any number of reasons.
pub struct VisualEvent {
    /// Where did the visual event originate?
    origin: GameObjectRef,

    /// A description of the visual event.
    description: String,
    /// A description of the event when it's visible in the distance.
    distant_description: String,
    /// A description of the event when it's barely visible.
    very_distant_description: String,

    /// Anyone who should not be notified of this event.
    pub excluded_characters: Vec<GameObjectRef>,
}

impl VisualEvent {
    /// Fires the event with the given strength.
    pub fn fire(&self, realm: &Realm, strength: f32) -> Option<Vec<PlayerOutput>> {
        visible_room_visitor::visit_rooms(realm, self, strength)
    }

    pub fn new(origin: GameObjectRef) -> Self {
        Self {
            origin,
            description: String::new(),
            distant_description: String::new(),
            very_distant_description: String::new(),
            excluded_characters: vec![],
        }
    }

    pub fn set_description(
        &mut self,
        description: &str,
        distant_description: &str,
        very_distant_description: &str,
    ) {
        self.description = description.to_owned();
        self.distant_description = distant_description.to_owned();
        self.very_distant_description = very_distant_description.to_owned();
    }
}

impl Event for VisualEvent {
    fn description_for_strength_and_character_in_room(
        &self,
        _: &Realm,
        strength: f32,
        _: GameObjectRef,
        _: GameObjectRef,
    ) -> Option<String> {
        Some(if strength > 0.9 {
            self.description.clone()
        } else if strength > 0.5 {
            self.distant_description.clone()
        } else {
            self.very_distant_description.clone()
        })
    }

    fn excluded_characters(&self) -> &[GameObjectRef] {
        &self.excluded_characters
    }

    fn origins(&self) -> Vec<GameObjectRef> {
        vec![self.origin]
    }
}
