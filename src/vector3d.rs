use std::borrow::Borrow;
use std::ops;

#[derive(Clone, Debug, Eq, PartialEq)]
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
