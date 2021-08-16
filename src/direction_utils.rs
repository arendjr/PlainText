use std::cmp::Ordering;

use crate::vector3d::Vector3D;

struct Direction {
    name: &'static str,
    opposite: &'static str,
    abbreviation: &'static str,
    vector: Vector3D,
}

impl Direction {
    const fn new(
        name: &'static str,
        opposite: &'static str,
        abbreviation: &'static str,
        vector: Vector3D,
    ) -> Self {
        Self {
            name,
            opposite,
            abbreviation,
            vector,
        }
    }
}

const DIRECTIONS: [Direction; 10] = [
    Direction::new("north", "south", "n", Vector3D::new(0, -1, 0)),
    Direction::new("northeast", "southwest", "ne", Vector3D::new(1, -1, 0)),
    Direction::new("east", "west", "e", Vector3D::new(1, 0, 0)),
    Direction::new("southeast", "northwest", "se", Vector3D::new(1, 1, 0)),
    Direction::new("south", "north", "s", Vector3D::new(0, 1, 0)),
    Direction::new("southwest", "northeast", "sw", Vector3D::new(-1, 1, 0)),
    Direction::new("west", "east", "w", Vector3D::new(-1, 0, 0)),
    Direction::new("northwest", "southeast", "nw", Vector3D::new(-1, -1, 0)),
    Direction::new("up", "down", "u", Vector3D::new(0, 0, 1)),
    Direction::new("down", "up", "d", Vector3D::new(0, 0, -1)),
];

pub fn compare_exit_names(name1: &&str, name2: &&str) -> Ordering {
    if let Some(index1) = index_of_direction(name1) {
        if let Some(index2) = index_of_direction(name2) {
            index1.cmp(&index2)
        } else {
            Ordering::Less
        }
    } else if is_direction(name2) {
        Ordering::Greater
    } else {
        name1.to_lowercase().cmp(&name2.to_lowercase())
    }
}

pub fn direction_by_abbreviation(string: &str) -> Option<&'static str> {
    index_of_direction_abbreviation(string).map(|index| DIRECTIONS[index].name)
}

fn index_of_direction(direction_str: &str) -> Option<usize> {
    for (i, direction) in DIRECTIONS.iter().enumerate() {
        if direction.name == direction_str {
            return Some(i);
        }
    }
    None
}

fn index_of_direction_abbreviation(direction_abbreviation: &str) -> Option<usize> {
    for (i, direction) in DIRECTIONS.iter().enumerate() {
        if direction.abbreviation == direction_abbreviation {
            return Some(i);
        }
    }
    None
}

pub fn is_direction(string: &str) -> bool {
    index_of_direction(string).is_some()
}

pub fn is_direction_abbreviation(string: &str) -> bool {
    index_of_direction_abbreviation(string).is_some()
}

pub fn vector_for_direction(direction: &str) -> Vector3D {
    if let Some(index) = index_of_direction(direction) {
        DIRECTIONS[index].vector.clone()
    } else {
        Vector3D::default()
    }
}
