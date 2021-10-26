use super::number_utils::{written_number, written_position};
use crate::{
    colors::Color,
    entity::{Entity, EntityRef, EntityType, Gender, ItemFlags, Realm},
};
use lazy_static::lazy_static;
use regex::{Captures, Regex};
use std::{borrow::Borrow, cmp::Ordering};

const COLOR_MAP: [&str; 16] = [
    "37;1", "37", "30;1", "30", "31;1", "31", "33;1", "33", "32;1", "32", "36;1", "36", "34;1",
    "34", "35;1", "35",
];

pub fn capitalize(string: &str) -> String {
    let mut chars: Vec<char> = string.chars().collect();
    if let Some(c) = chars.get(0) {
        chars[0] = c.to_ascii_uppercase();
    }
    chars.into_iter().collect()
}

pub fn colorize(string: &str, color: Color) -> String {
    format!("\x1B[{}m{}\x1B[0m", COLOR_MAP[color as usize], string)
}

fn count_entities<'a>(realm: &'a Realm, entity_refs: &[EntityRef]) -> Vec<(&'a dyn Entity, u16)> {
    let mut entities_with_counts = Vec::<(&dyn Entity, u16)>::new();
    for entity in entity_refs
        .iter()
        .filter_map(|entity_ref| realm.entity(*entity_ref))
    {
        if let Some(entity_with_count) = entities_with_counts
            .iter_mut()
            .find(|(other, _)| other.name() == entity.name())
        {
            entity_with_count.1 += 1;
        } else {
            entities_with_counts.push((entity, 1));
        }
    }

    entities_with_counts
}

fn count_and_sort_entities<'a>(
    realm: &'a Realm,
    entity_refs: &[EntityRef],
) -> Vec<(&'a dyn Entity, u16)> {
    let mut entities_with_counts = count_entities(realm, entity_refs);

    // Players go before everything else, but after that we sort by count:
    entities_with_counts.sort_by(|(entity1, count1), (entity2, count2)| {
        if entity1.entity_type() == EntityType::Player {
            if entity2.entity_type() == EntityType::Player {
                count1.cmp(count2)
            } else {
                Ordering::Less
            }
        } else if entity2.entity_type() == EntityType::Player {
            Ordering::Greater
        } else {
            count1.cmp(count2)
        }
    });

    entities_with_counts
}

pub fn definite_article_from_noun(noun: String) -> &'static str {
    if noun.chars().next().map(is_vowel).unwrap_or(false) {
        "an"
    } else {
        "a"
    }
}

pub fn definite_character_name(
    realm: &Realm,
    character_ref: EntityRef,
) -> Result<String, &'static str> {
    let (_, room) = realm.character_and_room_res(character_ref)?;
    definite_name(realm, character_ref, room.characters())
}

pub fn definite_name(
    realm: &Realm,
    subject: EntityRef,
    pool: &[EntityRef],
) -> Result<String, &'static str> {
    let subject = realm.entity_res(subject)?;
    if subject.indefinite_article().is_empty() {
        Ok(subject.name().to_owned())
    } else {
        let mut position = 0;
        let mut total = 0;
        for other in pool {
            let other = realm.entity_res(*other)?;
            if other.name() == subject.name() {
                total += 1;

                if other.id() == subject.id() {
                    position = total;
                }
            }
        }

        Ok(if total > 1 {
            format!("the {} {}", written_position(position), subject.name())
        } else {
            format!("the {}", subject.name())
        })
    }
}

pub fn describe_items(realm: &Realm, item_refs: &[EntityRef]) -> Vec<String> {
    count_and_sort_entities(realm, item_refs)
        .iter()
        .map(|(item, count)| {
            if *count > 1 {
                format!("{} {}", written_number(*count), item.plural_form())
            } else {
                item.indefinite_name()
            }
        })
        .collect()
}

pub fn describe_entities_from_room(
    realm: &Realm,
    entity_refs: &[EntityRef],
    room_ref: EntityRef,
) -> Vec<String> {
    count_and_sort_entities(realm, entity_refs)
        .iter()
        .map(|(entity, count)| {
            if *count > 1 {
                format!("{} {}", written_number(*count), entity.plural_form())
            } else if let Some(portal) = entity.as_portal() {
                portal.name_with_destination_from_room(room_ref)
            } else {
                entity.indefinite_name()
            }
        })
        .collect()
}

pub fn describe_entities_with_definite_articles(
    realm: &Realm,
    entity_refs: &[EntityRef],
) -> Vec<String> {
    count_and_sort_entities(realm, entity_refs)
        .iter()
        .map(|(entity, count)| {
            if *count > 1 {
                format!("{} {}", written_number(*count), entity.plural_form())
            } else {
                format!("the {}", entity.name())
            }
        })
        .collect()
}

pub fn first_item_is_plural(realm: &Realm, item_refs: &[EntityRef]) -> bool {
    match item_refs.first().and_then(|item_ref| realm.item(*item_ref)) {
        Some(item) => {
            if item.has_flags(ItemFlags::ImpliedPlural) {
                true
            } else if item.has_flags(ItemFlags::AlwaysUseDefiniteArticle) {
                item_refs.len() > 1
            } else {
                item_refs
                    .iter()
                    .skip(1)
                    .filter_map(|item_ref| realm.item(*item_ref))
                    .any(|other| other.name() == item.name())
            }
        }
        None => false,
    }
}

pub fn format_columns<T>(items: Vec<T>) -> String
where
    T: Borrow<str>,
{
    let mut result = String::new();
    let len = items.len();
    let half_len = len / 2 + len % 2;
    for i in 0..half_len {
        let first = items[i].borrow();
        let second = if i < len - half_len {
            items[i + half_len].borrow()
        } else {
            ""
        };

        result.push_str(&format!("  {:30}  {}\n", first, second));
    }
    result
}

pub fn format_weight(weight: f32) -> String {
    format!("{}kg", weight.round())
}

pub fn highlight(string: &str) -> String {
    colorize(string, Color::White)
}

pub fn is_letter(character: char) -> bool {
    ('a'..='z').contains(&character) || ('A'..='Z').contains(&character)
}

pub fn is_vowel(character: char) -> bool {
    character == 'a' || character == 'e' || character == 'i' || character == 'o' || character == 'u'
}

pub fn join_fancy<T>(list: &[T], separator: &str, last: &str) -> String
where
    T: Borrow<str>,
{
    let mut string = String::new();
    let len = list.len();
    for (i, item) in list.iter().enumerate() {
        string += item.borrow();
        match (i + 2).cmp(&len) {
            Ordering::Less => string += separator,
            Ordering::Equal => string += last,
            Ordering::Greater => {}
        }
    }
    string
}

pub fn join_sentence<T>(list: &[T]) -> String
where
    T: Borrow<str>,
{
    join_fancy(list, ", ", " and ")
}

pub fn object_pronoun(realm: &Realm, character_ref: EntityRef) -> &'static str {
    match realm.character(character_ref) {
        Some(character) => match realm.race(character.race()) {
            Some(race) if race.name() == "animal" => "it",
            _ => match character.gender() {
                Gender::Female => "her",
                Gender::Male => "him",
                Gender::Unspecified => "them",
            },
        },
        None => "them",
    }
}

pub fn plural_from_noun(noun: &str) -> String {
    if noun.ends_with('y') && !noun.chars().nth_back(1).map(is_vowel).unwrap_or(true) {
        format!("{}ies", substring(noun, 0, -1))
    } else if noun.ends_with('f') {
        format!("{}ves", substring(noun, 0, -1))
    } else if noun.ends_with("fe") {
        format!("{}ves", substring(noun, 0, -2))
    } else if noun.ends_with('s')
        || noun.ends_with('x')
        || noun.ends_with("sh")
        || noun.ends_with("ch")
    {
        format!("{}es", noun)
    } else if noun.ends_with("ese") {
        noun.to_owned()
    } else {
        format!("{}s", noun)
    }
}

pub fn possessive_adjective(realm: &Realm, character_ref: EntityRef) -> &'static str {
    match realm.character(character_ref) {
        Some(character) => match realm.race(character.race()) {
            Some(race) if race.name() == "animal" => "its",
            _ => match character.gender() {
                Gender::Female => "her",
                Gender::Male => "his",
                Gender::Unspecified => "their",
            },
        },
        None => "their",
    }
}

pub fn process_highlights(string: &str) -> String {
    lazy_static! {
        static ref HIGHLIGHT_REGEX: Regex = Regex::new(r"\*([^*]+)\*").unwrap();
    }

    (*HIGHLIGHT_REGEX.replace_all(string, |captures: &Captures| {
        highlight(captures.get(1).unwrap().as_str())
    }))
    .to_owned()
}

pub fn split_lines(string: &str, max_line_len: usize) -> Vec<String> {
    let mut lines = Vec::new();
    let mut current_line = String::new();
    let mut current_line_len = 0;

    for mut word in string.split(' ') {
        while let Some(index) = word.find('\n') {
            let left = &word[0..index];
            if !left.is_empty() {
                if current_line_len + left.len() >= max_line_len {
                    lines.push(current_line);
                    current_line = left.to_owned();
                } else {
                    if !current_line.is_empty() {
                        current_line += " ";
                    }
                    current_line += left;
                }
            }
            lines.push(current_line);
            current_line = String::new();
            current_line_len = 0;
            word = &word[index + 1..];
        }
        if word.is_empty() {
            continue;
        }

        let word_len = word.len() - 4 * word.matches("\x1B[").count();
        if current_line_len + word_len >= max_line_len {
            lines.push(current_line);
            current_line = word.to_owned();
            current_line_len = word_len;
        } else {
            if !current_line.is_empty() {
                current_line += " ";
                current_line_len += 1;
            }
            current_line += word;
            current_line_len += word_len;
        }
    }
    if !current_line.is_empty() {
        lines.push(current_line);
    }

    lines
}

pub fn subject_pronoun(realm: &Realm, character_ref: EntityRef) -> &'static str {
    match realm.character(character_ref) {
        Some(character) => match realm.race(character.race()) {
            Some(race) if race.name() == "animal" => "it",
            _ => match character.gender() {
                Gender::Female => "she",
                Gender::Male => "he",
                Gender::Unspecified => "they",
            },
        },
        None => "they",
    }
}

pub fn substring(string: &str, start: i32, end: i32) -> &str {
    &string[(start as usize)..if end > 0 {
        (end) as usize
    } else {
        ((string.len() as i32) + end) as usize
    }]
}
