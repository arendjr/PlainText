use serde::de::{self, Deserialize, Deserializer, SeqAccess, Visitor};
use serde::ser::{Serialize, SerializeTupleStruct, Serializer};
use std::borrow::Borrow;
use std::{fmt, ops};

#[derive(Clone, Debug, Default, Eq, PartialEq)]
pub struct Vector3D {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}

impl Vector3D {
    /**
     * Returns the angle between this vector and another.
     */
    pub fn angle(&self, other: &Self) -> f64 {
        let x = self.x as i64;
        let y = self.y as i64;
        let z = self.z as i64;
        let other_x = other.x as i64;
        let other_y = other.y as i64;
        let other_z = other.z as i64;
        let total_len = (self.len() as i64 * other.len() as i64) as f64;

        ((x * other_x + y * other_y + z * other_z) as f64 / total_len).acos()
    }

    pub fn is_default(&self) -> bool {
        self.x == 0 && self.y == 0 && self.z == 0
    }

    pub const fn new(x: i32, y: i32, z: i32) -> Self {
        Self { x, y, z }
    }

    /**
     * Returns the total length of this vector.
     */
    pub fn len(&self) -> i32 {
        let x = self.x as i64;
        let y = self.y as i64;
        let z = self.z as i64;
        ((x * x + y * y + z * z) as f32).sqrt() as i32
    }

    /**
     * Returns a normalized version of the vector.
     */
    pub fn normalized(&self) -> Self {
        let len = self.len();
        if len > 0 {
            Self::new(100 * self.x / len, 100 * self.y / len, 100 * self.z / len)
        } else {
            Self::new(0, 0, 0)
        }
    }
}

impl<T> ops::Add<T> for Vector3D
where
    T: Borrow<Self>,
{
    type Output = Self;

    fn add(self, rhs: T) -> Self::Output {
        Self {
            x: self.x + rhs.borrow().x,
            y: self.y + rhs.borrow().y,
            z: self.z + rhs.borrow().z,
        }
    }
}

impl<T> ops::Mul<T> for Vector3D
where
    T: Borrow<Self>,
{
    type Output = i32;

    fn mul(self, rhs: T) -> Self::Output {
        self.x * rhs.borrow().x + self.y * rhs.borrow().y + self.z * rhs.borrow().z
    }
}

impl<T> ops::Sub<T> for Vector3D
where
    T: Borrow<Self>,
{
    type Output = Self;

    fn sub(self, rhs: T) -> Self::Output {
        Self {
            x: self.x - rhs.borrow().x,
            y: self.y - rhs.borrow().y,
            z: self.z - rhs.borrow().z,
        }
    }
}

impl<'de> Deserialize<'de> for Vector3D {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        deserializer.deserialize_seq(Vector3DVisitor)
    }
}

impl fmt::Display for Vector3D {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Vector3D({}, {}, {})", self.x, self.y, self.z)
    }
}

impl Serialize for Vector3D {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let mut state = serializer.serialize_tuple_struct("Vector3D", 3)?;
        state.serialize_field(&self.x)?;
        state.serialize_field(&self.y)?;
        state.serialize_field(&self.z)?;
        state.end()
    }
}

struct Vector3DVisitor;

impl<'de> Visitor<'de> for Vector3DVisitor {
    type Value = Vector3D;

    fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "an array with three integers")
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
        Ok(Vector3D::new(x, y, z))
    }
}
