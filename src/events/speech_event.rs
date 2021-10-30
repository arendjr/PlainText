use super::{event::Event, sound_room_visitor};
use crate::{
    entity::{EntityRef, Gender, Realm},
    player_output::PlayerOutput,
    utils::{capitalize, definite_character_name, direction_for_vector, is_letter_or_number},
};

const EXCLUDED_CHARACTERS: [EntityRef; 0] = [];

/// An audible event that gets triggered when someone or something moves to another room.
pub struct SpeechEvent {
    /// Who is speaking?
    speaker: EntityRef,

    /// Where are they speaking from?
    origins: Vec<EntityRef>,

    /// What are they saying?
    message: String,

    /// Who are they saying it too?
    pub addressee: Option<EntityRef>,

    /// Is the person shouting?
    is_shout: bool,
}

impl SpeechEvent {
    /// Fires the event with the given strength.
    pub fn fire(&mut self, realm: &Realm, strength: f32) -> Option<Vec<PlayerOutput>> {
        let character = realm.character(self.speaker)?;
        self.origins.push(character.current_room());

        self.is_shout = strength > 2.0;

        if !self.message.ends_with(is_end_of_sentence_punctuation) {
            self.message.push(if self.is_shout { '!' } else { '.' });
        }

        sound_room_visitor::visit_rooms(realm, self, strength)
    }

    pub fn new(speaker: EntityRef, message: String) -> Self {
        Self {
            speaker,
            origins: Vec::new(),
            message,
            addressee: None,
            is_shout: false,
        }
    }
}

impl Event for SpeechEvent {
    fn description_for_strength_and_character_in_room(
        &self,
        realm: &Realm,
        strength: f32,
        _character: EntityRef,
        room: EntityRef,
    ) -> Option<String> {
        if strength >= 0.8 {
            let speaker_name = capitalize(&definite_character_name(realm, self.speaker).ok()?);
            let addressee_name = self
                .addressee
                .and_then(|addressee| definite_character_name(realm, addressee).ok());
            let description = if self.is_shout {
                format!("{} shouts, \"{}\"", speaker_name, self.message)
            } else if self.message.ends_with('?') {
                if let Some(addressee_name) = addressee_name {
                    format!(
                        "{} asks {}, \"{}\"",
                        speaker_name, addressee_name, self.message
                    )
                } else {
                    format!("{} asks, \"{}\"", speaker_name, self.message)
                }
            } else if let Some(addressee_name) = addressee_name {
                format!(
                    "{} says to {}, \"{}\"",
                    speaker_name, addressee_name, self.message
                )
            } else {
                format!("{} says, \"{}\"", speaker_name, self.message)
            };

            Some(description)
        } else if strength >= 0.5 {
            let mut garbled_words = Vec::new();
            let mut word = String::new();
            let mut num_heard_words = 0;
            for character in self.message.chars() {
                if character == ' ' {
                    if ((rand::random::<u8>() % 100) as f32) < 150.0 * (strength - 0.2) {
                        garbled_words.push(word);
                        num_heard_words += 1;
                    } else {
                        garbled_words.push(".".repeat(word.len()));
                    }
                    word = String::new();
                } else if is_letter_or_number(character) {
                    word.push(character);
                }
            }
            if ((rand::random::<u8>() % 100) as f32) < 150.0 * (strength - 0.2) {
                garbled_words.push(word);
                num_heard_words += 1;
            } else {
                garbled_words.push(".".repeat(word.len()));
            }

            let garbled_message = if num_heard_words == 0 {
                " something, but you cannot make out a word of it.".to_owned()
            } else {
                format!(", \"{}\"", garbled_words.join(" "))
            };

            let room = realm.room(room)?;
            let (speaker, origin) = realm.character_and_room_res(self.speaker).ok()?;
            let direction = direction_for_vector(&(origin.position() - room.position()));

            Some(format!(
                "You hear {} {} {}{}",
                match speaker.gender() {
                    Gender::Female => "a woman",
                    Gender::Male => "a man",
                    Gender::Unspecified => "someone",
                },
                match direction {
                    "up" => "from above".to_owned(),
                    "down" => "from below".to_owned(),
                    direction => format!("to the {}", direction),
                },
                if self.is_shout { "shouting" } else { "saying" },
                garbled_message
            ))
        } else {
            let what = if self.is_shout { "shout" } else { "mutter" };
            if strength >= 0.3 {
                let room = realm.room(room)?;
                let (_, origin) = realm.character_and_room_res(self.speaker).ok()?;
                let direction = direction_for_vector(&(origin.position() - room.position()));

                Some(match direction {
                    "up" => format!("You hear a distant {} from above.", what),
                    "down" => format!("You hear a distant {} from below.", what),
                    direction => format!("You hear a distant {} from the {}.", what, direction),
                })
            } else {
                Some(format!("You hear a distant {}.", what))
            }
        }
    }

    fn excluded_characters(&self) -> &[EntityRef] {
        &EXCLUDED_CHARACTERS
    }

    fn origins(&self) -> Vec<EntityRef> {
        self.origins.clone()
    }
}

pub fn is_end_of_sentence_punctuation(character: char) -> bool {
    character == '.' || character == '!' || character == '?'
}
