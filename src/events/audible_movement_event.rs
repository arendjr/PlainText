use std::f64::consts::TAU;

use crate::{
    direction_utils::direction_for_vector, game_object::GameObjectRef, objects::Realm,
    player_output::PlayerOutput, vector3d::Vector3D,
};

use super::{event::Event, sound_room_visitor};

const ONE_EIGHT_TAU: f64 = TAU / 8.0;
const THREE_EIGHT_TAU: f64 = 3.0 * ONE_EIGHT_TAU;

/// An audible event that gets triggered when someone or something moves to another room.
pub struct AudibleMovementEvent {
    /// Who are what is moving?
    subject: GameObjectRef,

    /// Where are they moving from?
    origin: GameObjectRef,
    /// Where are they moving to?
    destination: GameObjectRef,

    pub direction: Vector3D,

    /// Simple present of the verb describing the movement action.
    simple_present: String,
    /// Continuous form of the verb describing the movement action.
    continuous: String,
    /// Optional helper verb for describing the movement action.
    helper_verb: String,

    /// A description of the audible sound.
    description: String,
    /// A description of the sound when it's audible in the distance.
    distant_description: String,
    /// A description of the sound when it's barely audible.
    very_distant_description: String,

    /// Anyone who should not be notified of this event.
    pub excluded_characters: Vec<GameObjectRef>,
}

impl AudibleMovementEvent {
    /// Fires the event with the given strength.
    pub fn fire(&self, realm: &Realm, strength: f32) -> Option<Vec<PlayerOutput>> {
        sound_room_visitor::visit_rooms(realm, self, strength)
    }

    pub fn new(subject: GameObjectRef, origin: GameObjectRef, destination: GameObjectRef) -> Self {
        Self {
            subject,
            origin,
            destination,
            direction: Vector3D::default(),
            simple_present: String::new(),
            helper_verb: String::new(),
            continuous: String::new(),
            description: String::new(),
            distant_description: String::new(),
            very_distant_description: String::new(),
            excluded_characters: vec![subject],
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

    pub fn set_verb(&mut self, simple_present: &str, continuous: &str) {
        self.simple_present = simple_present.to_owned();

        if continuous.contains(' ') {
            let mut split = continuous.split(' ');
            self.helper_verb = split.next().unwrap().to_owned();
            self.continuous = split.next().unwrap().to_owned();
        }
    }
}

impl Event for AudibleMovementEvent {
    fn description_for_strength_and_character_in_room(
        &self,
        realm: &Realm,
        strength: f32,
        character: GameObjectRef,
        room: GameObjectRef,
    ) -> Option<String> {
        Some(if room == self.origin {
            format!("You hear {} {} away.", self.description, self.continuous)
        } else if room == self.destination {
            let character = realm.character(character)?;
            let origin = realm.room(self.origin)?;
            let room = realm.room(room)?;

            let vector = room.position() - origin.position();
            let mut angle = (vector.y as f64).atan2(vector.x as f64)
                - (character.direction().y as f64).atan2(character.direction().x as f64);
            if angle < -TAU / 2.0 {
                angle += TAU;
            } else if angle > TAU / 2.0 {
                angle -= TAU;
            }

            let direction = if !(-THREE_EIGHT_TAU..=THREE_EIGHT_TAU).contains(&angle) {
                "" // from the front
            } else if angle < ONE_EIGHT_TAU && angle > -ONE_EIGHT_TAU {
                " from behind"
            } else if angle > 0.0 {
                " from the left"
            } else {
                " from the right"
            };
            format!(
                "You hear {} {} up to you{}.",
                self.description, self.continuous, direction
            )
        } else {
            let origin = realm.room(self.origin)?;
            let room = realm.room(room)?;

            let description = if strength > 0.6 {
                &self.distant_description
            } else {
                &self.very_distant_description
            };

            let direction = if strength > 0.8 {
                let vector = room.position() - origin.position();
                let angle = self.direction.angle(&vector);
                if angle < TAU / 4.0 {
                    "toward you"
                } else {
                    "away from you"
                }
                .to_owned()
            } else {
                let vector = origin.position() - room.position();
                format!("to the {}", direction_for_vector(&vector))
            };

            format!(
                "You hear {} {} {}.",
                description, self.continuous, direction
            )
        })
    }

    fn excluded_characters(&self) -> &[GameObjectRef] {
        &self.excluded_characters
    }

    fn origin(&self) -> GameObjectRef {
        self.origin
    }
}
