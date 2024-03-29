use serde::de::{self, Deserialize, Deserializer, SeqAccess, Visitor};
use serde::ser::{Serialize, SerializeTupleStruct, Serializer};
use std::borrow::Borrow;
use std::{fmt, ops};

use crate::vector3d::Vector3D;

#[derive(Clone, Debug, Default)]
pub struct Point3D {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}

impl Point3D {
    pub fn from_str(point_str: &str) -> Result<Self, String> {
        let is_parenthesized = point_str.starts_with('(') && point_str.ends_with(')');
        let is_square_bracketed = point_str.starts_with('[') && point_str.ends_with(']');
        if !is_parenthesized && !is_square_bracketed {
            return Err(format!(
                "Invalid point -- use parenthesis or square brackets to denote points (received: {})",
                point_str
            ));
        }

        let parts: Vec<&str> = point_str[1..point_str.len() - 1].split(',').collect();
        if parts.len() != 3 {
            return Err("Invalid point -- must contain 3 comma-separated coordinates".to_owned());
        }

        Ok(Self {
            x: parts[0]
                .parse()
                .map_err(|err| format!("Could not parse x coordinate: {:?}", err))?,
            y: parts[1]
                .parse()
                .map_err(|err| format!("Could not parse y coordinate: {:?}", err))?,
            z: parts[2]
                .parse()
                .map_err(|err| format!("Could not parse z coordinate: {:?}", err))?,
        })
    }

    pub fn is_default(&self) -> bool {
        self.x == 0 && self.y == 0 && self.z == 0
    }

    pub fn to_vec(&self) -> Vector3D {
        Vector3D::new(self.x, self.y, self.z)
    }
}

impl<T> ops::Sub<T> for Point3D
where
    T: Borrow<Self>,
{
    type Output = Vector3D;

    fn sub(self, rhs: T) -> Self::Output {
        Vector3D::new(
            self.x - rhs.borrow().x,
            self.y - rhs.borrow().y,
            self.z - rhs.borrow().z,
        )
    }
}

impl<T> ops::Sub<T> for &Point3D
where
    T: Borrow<Self>,
{
    type Output = Vector3D;

    fn sub(self, rhs: T) -> Self::Output {
        Vector3D::new(
            self.x - rhs.borrow().x,
            self.y - rhs.borrow().y,
            self.z - rhs.borrow().z,
        )
    }
}

impl<'de> Deserialize<'de> for Point3D {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        deserializer.deserialize_seq(Point3DVisitor)
    }
}

impl fmt::Display for Point3D {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Point3D({}, {}, {})", self.x, self.y, self.z)
    }
}

impl Serialize for Point3D {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let mut state = serializer.serialize_tuple_struct("Point3D", 3)?;
        state.serialize_field(&self.x)?;
        state.serialize_field(&self.y)?;
        state.serialize_field(&self.z)?;
        state.end()
    }
}

struct Point3DVisitor;

impl<'de> Visitor<'de> for Point3DVisitor {
    type Value = Point3D;

    fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        formatter.write_str("an array with three integers")
    }

    fn visit_seq<A>(self, mut seq: A) -> Result<Self::Value, A::Error>
    where
        A: SeqAccess<'de>,
    {
        let x = seq
            .next_element()?
            .ok_or_else(|| de::Error::invalid_length(0, &self))?;
        let y = seq
            .next_element()?
            .ok_or_else(|| de::Error::invalid_length(1, &self))?;
        let z = seq
            .next_element()?
            .ok_or_else(|| de::Error::invalid_length(2, &self))?;
        Ok(Point3D { x, y, z })
    }
}
