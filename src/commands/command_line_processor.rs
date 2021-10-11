use crate::{
    entity::{EntityRef, Realm},
    number_utils::numeric_position,
};
use std::collections::VecDeque;

#[derive(Clone, Debug)]
pub struct EntityDescription {
    pub name: String,
    pub position: u16,
    pub container: Option<Box<EntityDescription>>,
}

impl EntityDescription {
    pub fn new(position: u16, name: String) -> Self {
        Self {
            name,
            position,
            container: None,
        }
    }
}

pub struct CommandLineProcessor {
    player_ref: EntityRef,
    words: VecDeque<String>,
}

impl CommandLineProcessor {
    fn description_predicate<'a>(
        &self,
        realm: &'a Realm,
        description: EntityDescription,
    ) -> Box<dyn FnMut(&&EntityRef) -> bool + 'a> {
        let character = match realm.character(self.player_ref) {
            Some(character) => character,
            None => return Box::new(|_| false),
        };

        Box::new(move |entity_ref| {
            let entity = match realm.entity(**entity_ref) {
                Some(entity) => entity,
                None => return false,
            };
            let name = if let Some(portal) = entity.as_portal() {
                portal.name_from_room(character.current_room())
            } else {
                entity.name()
            }
            .to_lowercase();

            if description.name.contains(' ') {
                name.starts_with(&description.name)
            } else {
                name.split(' ')
                    .any(|word| word.starts_with(&description.name))
            }
        })
    }

    pub fn entities_by_description(
        &self,
        realm: &Realm,
        pool: &[EntityRef],
        description: EntityDescription,
    ) -> Vec<EntityRef> {
        let position = description.position;
        let entities: Vec<EntityRef> = if position == 0 && description.name == "all" {
            pool.iter().copied().collect()
        } else {
            pool.iter()
                .filter(self.description_predicate(realm, description))
                .copied()
                .collect()
        };

        if position > 0 {
            if position <= entities.len() as u16 {
                vec![entities[position as usize - 1]]
            } else {
                vec![]
            }
        } else {
            entities
        }
    }

    pub fn entity_by_description(
        &self,
        realm: &Realm,
        pool: &[EntityRef],
        description: EntityDescription,
    ) -> Option<EntityRef> {
        if description.position > 0 {
            let index = description.position as usize - 1;
            pool.iter()
                .filter(self.description_predicate(realm, description))
                .nth(index)
                .copied()
        } else {
            pool.iter()
                .find(self.description_predicate(realm, description))
                .copied()
        }
    }

    pub fn has_words_left(&self) -> bool {
        !self.words.is_empty()
    }

    pub fn new(player_ref: EntityRef, command_line: &str) -> Self {
        let mut words = VecDeque::new();
        let mut current_word = String::new();
        let mut is_quoted = false;

        for character in command_line.chars() {
            if character.is_whitespace() {
                if is_quoted {
                    current_word.push(character);
                } else if !current_word.is_empty() {
                    words.push_back(current_word);
                    current_word = String::new();
                }
            } else if character == '"' {
                if is_quoted || current_word.is_empty() {
                    is_quoted = !is_quoted;
                } else {
                    current_word.push('"');
                }
            } else {
                current_word.push(character);
            }
        }
        if !current_word.is_empty() {
            words.push_back(current_word);
        }

        Self { player_ref, words }
    }

    pub fn num_words_left(&self) -> usize {
        self.words.len()
    }

    pub fn peek_rest(&self) -> String {
        self.words.iter().fold(String::new(), |mut rest, word| {
            if !rest.is_empty() {
                rest.push(' ');
            }
            rest.push_str(word);
            rest
        })
    }

    pub fn peek_word(&self) -> Option<&str> {
        self.words.front().map(String::as_ref)
    }

    pub fn prepend_word(&mut self, word: String) {
        self.words.push_front(word);
    }

    pub fn skip_connecting_word(&mut self, word: &str) {
        if let Some(first_word) = self.words.front() {
            if first_word == word && self.words.len() >= 2 {
                self.words.pop_front();
            }
        }
    }

    pub fn skip_word(&mut self) {
        self.words.pop_front();
    }

    pub fn take_entities(&mut self, realm: &Realm, pool: &[EntityRef]) -> Vec<EntityRef> {
        if let Some(description) = self.take_entity_description() {
            self.entities_by_description(realm, pool, description)
        } else {
            vec![]
        }
    }

    pub fn take_entity_description(&mut self) -> Option<EntityDescription> {
        self.skip_connecting_word("the");

        let mut name = self.take_word()?.to_lowercase();
        let mut position = 0;

        if let Some(dot_index) = name.chars().position(|c| c == '.') {
            if let Ok(pos) = name[0..dot_index].parse() {
                position = pos;
                name = name.chars().skip(dot_index + 1).collect();
            }
        } else if let Some(next_word) = self.peek_word() {
            if let Some(pos) = numeric_position(&name) {
                position = pos;
                name = next_word.to_lowercase();
                self.skip_word();
            } else if let Ok(pos) = next_word.parse() {
                position = pos;
                self.skip_word();
            }
        }

        Some(EntityDescription::new(position, name))
    }

    pub fn take_entity(&mut self, realm: &Realm, pool: &[EntityRef]) -> Option<EntityRef> {
        if let Some(description) = self.take_entity_description() {
            self.entity_by_description(realm, pool, description)
        } else {
            None
        }
    }

    pub fn take_rest(&mut self) -> String {
        self.words.drain(0..).fold("".to_owned(), |rest, word| {
            if rest.is_empty() {
                word
            } else {
                format!("{} {}", rest, word)
            }
        })
    }

    pub fn take_word(&mut self) -> Option<String> {
        self.words.pop_front()
    }
}

#[cfg(test)]
mod tests {
    use crate::entity::{EntityRef, EntityType};

    use super::CommandLineProcessor;

    #[test]
    fn split_words() {
        let player_ref = EntityRef(EntityType::Player, 1);

        let mut p1 = CommandLineProcessor::new(player_ref, "look  north");
        assert_eq!(&p1.take_word().unwrap(), "look");
        assert_eq!(&p1.take_word().unwrap(), "north");
        assert_eq!(p1.num_words_left(), 0);

        let mut p2 = CommandLineProcessor::new(player_ref, " look\tat the window ");
        assert_eq!(&p2.take_word().unwrap(), "look");
        assert_eq!(&p2.take_word().unwrap(), "at");
        assert_eq!(&p2.take_word().unwrap(), "the");
        assert_eq!(&p2.take_word().unwrap(), "window");
        assert_eq!(p2.num_words_left(), 0);

        let mut p3 = CommandLineProcessor::new(player_ref, "look at the \"open window\"");
        assert_eq!(&p3.take_word().unwrap(), "look");
        assert_eq!(&p3.take_word().unwrap(), "at");
        assert_eq!(&p3.take_word().unwrap(), "the");
        assert_eq!(&p3.take_word().unwrap(), "open window");
        assert_eq!(p3.num_words_left(), 0);
    }
}
