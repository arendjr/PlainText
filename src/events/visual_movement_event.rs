use std::f64::consts::TAU;

use crate::{
    direction_utils::{direction_for_vector, is_direction},
    entity::{Entity, EntityRef, EntityType, Gender, Realm},
    player_output::PlayerOutput,
    text_utils::{capitalize, definite_character_name, join_sentence},
    vector3d::Vector3D,
};

use super::{event::Event, visible_room_visitor};

/// A visual event that gets triggered when someone or something moves to another room.
pub struct VisualMovementEvent {
    /// Who are what is moving?
    subject: EntityRef,

    /// Where are they moving from?
    origin: EntityRef,
    /// Where are they moving to?
    destination: EntityRef,

    pub direction: Vector3D,

    /// Simple present of the verb describing the movement action.
    simple_present: String,
    /// Continuous form of the verb describing the movement action.
    continuous: String,
    /// Optional helper verb for describing the movement action.
    helper_verb: String,

    /// Anyone who should not be notified of this event.
    pub excluded_characters: Vec<EntityRef>,
}

impl VisualMovementEvent {
    /// Fires the event with the given strength.
    pub fn fire(&self, realm: &Realm, strength: f32) -> Option<Vec<PlayerOutput>> {
        visible_room_visitor::visit_rooms(realm, self, strength)
    }

    pub fn new(subject: EntityRef, origin: EntityRef, destination: EntityRef) -> Self {
        Self {
            subject,
            origin,
            destination,
            direction: Vector3D::default(),
            simple_present: String::new(),
            helper_verb: String::new(),
            continuous: String::new(),
            excluded_characters: vec![],
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
        _: EntityRef,
        room: EntityRef,
    ) -> Option<String> {
        let subject = name_at_strength(realm, self.subject, strength)?;

        if room == self.origin {
            let room = realm.room(room)?;
            for portal in room.portals().iter() {
                let portal = match realm.portal(*portal) {
                    Some(portal) => portal,
                    None => continue,
                };

                if portal.room() == self.destination || portal.room2() == self.destination {
                    let exit_name = portal.name_from_room(room.entity_ref());
                    return Some(if is_direction(exit_name) {
                        format!(
                            "{} {} {}.",
                            capitalize(&subject),
                            self.simple_present,
                            exit_name
                        )
                    } else if exit_name == "out" {
                        format!("{} {} outside.", capitalize(&subject), self.simple_present)
                    } else if portal.can_open_from_room(room.entity_ref()) {
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

    fn excluded_characters(&self) -> &[EntityRef] {
        &self.excluded_characters
    }

    fn origins(&self) -> Vec<EntityRef> {
        vec![self.origin, self.destination]
    }
}

/// Returns the name of the entity or a more fuzzy description, depending on the strength
/// (clarity) with which the entity is being observed.
///
/// The clarity with which the entity is being observed is indicated through the strength,
/// where 1.0 represents a full clear view, while 0.0 means the entity has become invisible.
fn name_at_strength(realm: &Realm, subject: EntityRef, strength: f32) -> Option<String> {
    match subject.entity_type() {
        EntityType::Group => group_name_at_strength(realm, subject, strength),
        EntityType::Npc | EntityType::Player => {
            let character = realm.entity(subject)?;
            Some(if strength >= 0.9 {
                character.indefinite_name()
            } else if strength >= 0.8 {
                match character.as_character()?.gender() {
                    Gender::Male => "a man",
                    Gender::Female => "a woman",
                    Gender::Unspecified => "someone",
                }
                .to_owned()
            } else {
                "someone".to_owned()
            })
        }
        _ => Some(if strength >= 1.0 {
            realm.entity(subject)?.name().to_owned()
        } else {
            "something".to_owned()
        }),
    }
}

fn group_name_at_strength(realm: &Realm, group: EntityRef, strength: f32) -> Option<String> {
    let group = realm.group(group)?;
    let leader_ref = group.leader();
    let leader = realm.character(leader_ref)?;
    let room = realm.room(leader.current_room())?;

    let mut party = vec![leader_ref];
    for follower in group.followers().iter() {
        let is_in_same_room = realm
            .character(*follower)
            .map(|follower| follower.current_room() == room.entity_ref())
            .unwrap_or(false);
        if is_in_same_room {
            party.push(*follower);
        }
    }

    Some(if strength > 0.9 {
        join_sentence(
            &party
                .iter()
                .filter_map(|character| definite_character_name(realm, *character).ok())
                .collect::<Vec<_>>(),
        )
    } else if strength > 0.8 {
        let mut num_males = 0;
        let mut num_females = 0;
        let mut num_others = 0;
        for character in party.iter() {
            let character = realm.character(*character)?;
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
