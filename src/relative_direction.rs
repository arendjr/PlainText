use std::fmt::Display;

use crate::vector3d::Vector3D;

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum RelativeDirection {
    Ahead,
    Behind,
    Left,
    Right,
    Up,
    Down,
}

impl RelativeDirection {
    pub fn from(&self, reference: &Vector3D) -> Vector3D {
        match self {
            Self::Ahead => Vector3D::new(reference.x, reference.y, 0),
            Self::Behind => Vector3D::new(-reference.x, -reference.y, 0),
            Self::Left => Vector3D::new(reference.y, -reference.x, 0),
            Self::Right => Vector3D::new(-reference.y, reference.x, 0),
            Self::Up => Vector3D::new(reference.x, reference.y, 1),
            Self::Down => Vector3D::new(reference.x, reference.y, -1),
        }
    }

    pub fn from_string(direction: &str) -> Option<Self> {
        match direction {
            "ahead" => Some(Self::Ahead),
            "back" => Some(Self::Behind),
            "behind" => Some(Self::Behind),
            "forward" => Some(Self::Ahead),
            "left" => Some(Self::Left),
            "right" => Some(Self::Right),
            "up" => Some(Self::Up),
            "down" => Some(Self::Down),
            _ => None,
        }
    }
}

impl Display for RelativeDirection {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_str(match self {
            Self::Ahead => "ahead",
            Self::Behind => "behind",
            Self::Left => "left",
            Self::Right => "right",
            Self::Up => "up",
            Self::Down => "down",
        })
    }
}
