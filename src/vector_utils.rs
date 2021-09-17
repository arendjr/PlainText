use std::f64::consts::{PI, TAU};

use crate::vector3d::Vector3D;

pub fn angle_between_xy_vectors(v1: &Vector3D, v2: &Vector3D) -> f64 {
    let mut angle = (v1.y as f64).atan2(v1.x as f64) - (v2.y as f64).atan2(v2.x as f64);
    if angle < -PI {
        angle += TAU;
    } else if angle > PI {
        angle -= TAU;
    }
    angle
}
