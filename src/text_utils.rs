use lazy_static::lazy_static;
use regex::{Captures, Regex};
use std::borrow::Borrow;

use crate::colors::Color;
use crate::game_object::{GameObject, GameObjectRef, SharedGameObject};
use crate::objects::{ItemFlags, Realm};

const COLOR_MAP: [&'static str; 16] = [
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

fn count_objects(
    realm: &Realm,
    object_refs: &Vec<GameObjectRef>,
) -> Vec<(SharedGameObject, usize)> {
    let mut objects_with_counts = Vec::<(SharedGameObject, usize)>::new();
    for object in object_refs
        .iter()
        .filter_map(|object_ref| realm.object(*object_ref))
    {
        if let Some(object_with_count) = objects_with_counts
            .iter_mut()
            .find(|(other, _)| other.name() == object.name())
        {
            object_with_count.1 += 1;
        } else {
            objects_with_counts.push((object, 1));
        }
    }
    objects_with_counts
}

pub fn definite_article_from_noun(noun: String) -> &'static str {
    if noun.chars().next().map(is_vowel).unwrap_or(false) {
        "an"
    } else {
        "a"
    }
}

pub fn describe_items_from_room(
    realm: &Realm,
    object_refs: &Vec<GameObjectRef>,
    room_ref: GameObjectRef,
) -> Vec<String> {
    count_objects(realm, object_refs)
        .iter()
        .map(|(object, count)| {
            if *count > 1 {
                format!("{} {}", written_number(*count), object.plural_form())
            } else if let Some(item) = object.as_item() {
                if item.has_flags(ItemFlags::AlwaysUseDefiniteArticle) {
                    format!("the {}", object.name())
                } else if item.has_flags(ItemFlags::ImpliedPlural) {
                    item.name().to_owned()
                } else {
                    item.indefinite_name()
                }
            } else if let Some(portal) = object.as_portal() {
                portal.name_with_destination_from_room(room_ref)
            } else {
                object.indefinite_name()
            }
        })
        .collect()
}

pub fn describe_items_with_definite_articles(
    realm: &Realm,
    object_refs: &Vec<GameObjectRef>,
) -> Vec<String> {
    count_objects(realm, object_refs)
        .iter()
        .map(|(object, count)| {
            if *count > 1 {
                format!("{} {}", written_number(*count), object.plural_form())
            } else {
                format!("the {}", object.name())
            }
        })
        .collect()
}

pub fn first_item_is_plural(realm: &Realm, item_refs: &Vec<GameObjectRef>) -> bool {
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

pub fn highlight(string: &str) -> String {
    colorize(string, Color::White)
}

pub fn is_letter(character: char) -> bool {
    character >= 'a' && character <= 'z' || character >= 'A' && character <= 'Z'
}

pub fn is_vowel(character: char) -> bool {
    character == 'a' || character == 'e' || character == 'i' || character == 'o' || character == 'u'
}

pub fn join_fancy(list: Vec<&str>, separator: &str, last: &str) -> String {
    let mut string = String::new();
    let len = list.len();
    for i in 0..len {
        string += list[i];
        if i + 2 < len {
            string += separator;
        } else if i + 2 == len {
            string += last;
        }
    }
    return string;
}

pub fn join_sentence(list: Vec<&str>) -> String {
    join_fancy(list, ", ", " and ")
}

pub fn plural_from_noun(noun: &str) -> String {
    if noun.ends_with("y") && !noun.chars().nth_back(1).map(is_vowel).unwrap_or(true) {
        format!("{}ies", substring(noun, 0, -1))
    } else if noun.ends_with("f") {
        format!("{}ves", substring(noun, 0, -1))
    } else if noun.ends_with("fe") {
        format!("{}ves", substring(noun, 0, -2))
    } else if noun.ends_with("s")
        || noun.ends_with("x")
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
                if current_line_len + left.len() + 1 > max_line_len {
                    lines.push(current_line);
                    current_line = left.to_owned();
                    current_line_len = left.len();
                } else {
                    if !current_line.is_empty() {
                        current_line += " ";
                        current_line_len += 1;
                    }
                    current_line += left;
                    current_line_len += left.len();
                }
            }
            lines.push(current_line);
            current_line = "".to_owned();
            word = &word[index + 1..];
        }
        if word.is_empty() {
            continue;
        }

        let word_len = word.len() - 4 * word.matches("\x1B[").count();
        if current_line_len + word_len + 1 > max_line_len {
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

fn substring(string: &str, start: i32, end: i32) -> &str {
    &string[(start as usize)..if end > 0 {
        (end - start) as usize
    } else {
        ((string.len() as i32) - end - start) as usize
    }]
}

const NUM_WRITTEN_NUMBERS: usize = 20;
const WRITTEN_NUMBERS: [&'static str; NUM_WRITTEN_NUMBERS] = [
    "zero",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "sevens",
    "eight",
    "nine",
    "ten",
    "eleven",
    "twelve",
    "thirteen",
    "fourteen",
    "fifteen",
    "sixteen",
    "seventeen",
    "eighteen",
    "nineteen",
];

pub fn written_number(number: usize) -> String {
    if number < NUM_WRITTEN_NUMBERS {
        WRITTEN_NUMBERS[number].to_owned()
    } else {
        number.to_string()
    }
}
