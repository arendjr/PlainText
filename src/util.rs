pub fn get_definite_article_from_noun(noun: String) -> &'static str {
    if noun.chars().next().map(is_vowel).unwrap_or(false) {
        "an"
    } else {
        "a"
    }
}

fn get_plural_from_noun(noun: String) -> String {
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
        noun.clone()
    } else {
        format!("{}s", noun)
    }
}

pub fn is_vowel(character: char) -> bool {
    character == 'a' || character == 'e' || character == 'i' || character == 'o' || character == 'u'
}

fn substring(string: String, start: i32, end: i32) -> String {
    string
        .chars()
        .into_iter()
        .skip(start as usize)
        .take(if end > 0 {
            (end - start) as usize
        } else {
            ((string.len() as i32) - end - start) as usize
        })
        .collect::<String>()
}
