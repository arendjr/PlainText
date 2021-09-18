use std::f64::consts::TAU;

use crate::{
    direction_utils::{direction_for_vector, is_direction},
    game_object::{GameObject, GameObjectRef},
    objects::Realm,
    player_output::PlayerOutput,
    text_utils::capitalize,
    vector3d::Vector3D,
};

use super::{event::Event, visible_room_visitor};

/// A visual event that gets triggered when someone or something moves to another room.
pub struct VisualMovementEvent {
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

    /// Anyone who should not be notified of this event.
    pub excluded_characters: Vec<GameObjectRef>,
}

impl VisualMovementEvent {
    /// Fires the event with the given strength.
    pub fn fire(&self, realm: &Realm, strength: f32) -> Option<Vec<PlayerOutput>> {
        visible_room_visitor::visit_rooms(realm, self, strength)
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
            excluded_characters: vec![subject],
        }
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

impl Event for VisualMovementEvent {
    fn description_for_strength_and_character_in_room(
        &self,
        realm: &Realm,
        strength: f32,
        _: GameObjectRef,
        room: GameObjectRef,
    ) -> Option<String> {
        let subject = realm.object(self.subject)?.name_at_strength(strength);

        if room == self.origin {
            let room = realm.room(room)?;
            for portal in room.portals().iter() {
                let portal = match realm.portal(*portal) {
                    Some(portal) => portal,
                    None => continue,
                };

                if portal.room() == self.destination || portal.room2() == self.destination {
                    let exit_name = portal.name_from_room(room.object_ref());
                    return Some(if is_direction(exit_name) {
                        format!(
                            "{} {} {}.",
                            capitalize(&subject),
                            self.simple_present,
                            exit_name
                        )
                    } else if exit_name == "out" {
                        format!("{} {} outside.", capitalize(&subject), self.simple_present)
                    } else if portal.can_open_from_room(room.object_ref()) {
                        format!(
                            "{} {} through the {}.",
                            capitalize(&subject),
                            self.simple_present,
                            exit_name
                        )
                    } else {
                        format!(
                            "{} {} to the {}.",
                            capitalize(&subject),
                            self.simple_present,
                            exit_name
                        )
                    });
                }
            }

            return Some(format!(
                "{} {} {}.",
                capitalize(&subject),
                self.simple_present,
                direction_for_vector(&self.direction)
            ));
        } else if room == self.destination {
            return Some(format!(
                "{} {} up to you.",
                capitalize(&subject),
                self.simple_present
            ));
        }

        let room = realm.room(room)?;
        let origin = realm.room(self.origin)?;
        let vector = room.position() - origin.position();

        let distance = vector.len();
        let prefix = if distance > 100 {
            match rand::random::<u8>() % 5 {
                0 => "In the distance,",
                1 => "In the distance, you see",
                2 => "In the distance, you can see",
                3 => "From afar, you see",
                4 => "From afar, you can see",
                _ => "You see",
            }
        } else {
            "You see"
        };

        let helper_verb = if prefix.ends_with(',') {
            format!("{} ", self.helper_verb)
        } else {
            "".to_owned()
        };

        let angle = self.direction.angle(&vector);
        let direction = if angle < TAU / 8.0 {
            if distance > 100 {
                "in your direction"
            } else {
                "toward you"
            }
        } else if angle >= TAU * 3.0 / 8.0 && distance <= 100 {
            "away from you"
        } else {
            direction_for_vector(&self.direction)
        };

        Some(format!(
            "{} {} {}{} {}.",
            prefix, subject, helper_verb, self.continuous, direction
        ))
    }

    fn excluded_characters(&self) -> &[GameObjectRef] {
        &self.excluded_characters
    }

    fn origins(&self) -> Vec<GameObjectRef> {
        vec![self.origin, self.destination]
    }
}
