use std::{cmp::Ordering, f64::consts::TAU};

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

pub fn direction_for_vector(vector: &Vector3D) -> &'static str {
    if vector.z.abs() as f64 > ((vector.x * vector.x + vector.y * vector.y) as f64).sqrt() {
        if vector.z > 0 {
            DIRECTIONS[9].name
        } else {
            DIRECTIONS[8].name
        }
    } else {
        let degrees = (vector.y as f64).atan2(vector.x as f64) * 360.0 / TAU;
        if (-112.5..-67.5).contains(&degrees) {
            DIRECTIONS[0].name
        } else if (-67.5..-22.5).contains(&degrees) {
            DIRECTIONS[1].name
        } else if (-22.5..22.5).contains(&degrees) {
            DIRECTIONS[2].name
        } else if (22.5..67.5).contains(&degrees) {
            DIRECTIONS[3].name
        } else if (67.5..112.5).contains(&degrees) {
            DIRECTIONS[4].name
        } else if (112.5..157.5).contains(&degrees) {
            DIRECTIONS[5].name
        } else if !(-157.5..=157.5).contains(&degrees) {
            DIRECTIONS[6].name
        } else {
            DIRECTIONS[7].name
        }
    }
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

pub fn vector_for_direction(direction: &str) -> Option<Vector3D> {
    if let Some(index) = index_of_direction(direction) {
        Some(DIRECTIONS[index].vector.clone())
    } else {
        index_of_direction_abbreviation(direction).map(|index| DIRECTIONS[index].vector.clone())
    }
}
