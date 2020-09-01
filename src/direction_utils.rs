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

fn index_of_direction(string: &str) -> Option<usize> {
    for i in 0..DIRECTIONS.len() {
        if DIRECTIONS[i].name == string {
            return Some(i);
        }
    }
    None
}

fn index_of_direction_abbreviation(string: &str) -> Option<usize> {
    for i in 0..DIRECTIONS.len() {
        if DIRECTIONS[i].abbreviation == string {
            return Some(i);
        }
    }
    None
}

pub fn get_direction_by_abbreviation(string: &str) -> Option<&'static str> {
    index_of_direction_abbreviation(string).map(|index| DIRECTIONS[index].name)
}

pub fn is_direction(string: &str) -> bool {
    index_of_direction(string).is_some()
}

pub fn is_direction_abbreviation(string: &str) -> bool {
    index_of_direction_abbreviation(string).is_some()
}
