use bitflags::bitflags;
use lazy_static::lazy_static;
use regex::{Captures, Regex};
use std::borrow::Borrow;

use crate::colors::Color;

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

pub fn definite_article_from_noun(noun: String) -> &'static str {
    if noun.chars().next().map(is_vowel).unwrap_or(false) {
        "an"
    } else {
        "a"
    }
}

bitflags! {
    pub struct FormatOptions: u32 {
        const None = 0b00000000;
        const Capitalized = 0b00000001;
        const Highlighted = 0b00000010;
    }
}

pub fn format_columns<T>(items: Vec<T>, options: FormatOptions) -> String
where
    T: Borrow<str>,
{
    let mut result = String::new();
    let len = items.len();
    let half_len = len / 2 + len % 2;
    for i in 0..half_len {
        let mut first = items[i].borrow().to_owned();
        let mut second = if i < len - half_len {
            items[i + half_len].borrow().to_owned()
        } else {
            "".to_owned()
        };

        if options & FormatOptions::Capitalized != FormatOptions::None {
            first = capitalize(&first);
            second = capitalize(&second);
        }
        if options & FormatOptions::Highlighted != FormatOptions::None {
            first = highlight(&first);
            second = highlight(&second);
        }

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
