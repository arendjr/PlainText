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
            "behind" => Some(Self::Behind),
            "left" => Some(Self::Left),
            "right" => Some(Self::Right),
            "up" => Some(Self::Up),
            "down" => Some(Self::Down),
            _ => None,
        }
    }
}
