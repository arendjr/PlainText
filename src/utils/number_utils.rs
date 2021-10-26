const NUM_WRITTEN_NUMBERS: u16 = 20;
const WRITTEN_NUMBERS: [&str; NUM_WRITTEN_NUMBERS as usize] = [
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

const NUM_WRITTEN_POSITIONS: u16 = 20;
const WRITTEN_POSITIONS: [&str; NUM_WRITTEN_POSITIONS as usize] = [
    "zeroth",
    "first",
    "second",
    "third",
    "fourth",
    "fifth",
    "sixth",
    "seventh",
    "eightth",
    "ninth",
    "tenth",
    "eleventh",
    "twelfth",
    "thirteenth",
    "fourteenth",
    "fifteenth",
    "sixteenth",
    "seventeenth",
    "eighteenth",
    "nineteenth",
];

pub fn numeric_position(position: &str) -> Option<u16> {
    for i in 0..NUM_WRITTEN_POSITIONS {
        if WRITTEN_POSITIONS[i as usize] == position {
            return Some(i);
        }
    }

    if position.ends_with("1st")
        || position.ends_with("2nd")
        || position.ends_with("3rd")
        || position.ends_with("th")
    {
        position[0..position.len() - 2].parse().ok()
    } else {
        None
    }
}

pub fn written_number(number: u16) -> String {
    if number < NUM_WRITTEN_NUMBERS {
        WRITTEN_NUMBERS[number as usize].to_owned()
    } else {
        number.to_string()
    }
}

pub fn written_position(position: u16) -> String {
    if position < NUM_WRITTEN_POSITIONS {
        WRITTEN_POSITIONS[position as usize].to_owned()
    } else {
        let rest = position % 10;
        let suffix = if rest == 1 {
            "st"
        } else if rest == 2 {
            "nd"
        } else if rest == 3 {
            "rd"
        } else {
            "th"
        };
        position.to_string() + suffix
    }
}
