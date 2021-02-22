use pbkdf2::{pbkdf2_check, pbkdf2_simple};
use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::character_stats::CharacterStats;
use crate::game_object::{
    Character, GameObject, GameObjectId, GameObjectRef, GameObjectType, Gender, SharedGameObject,
};
use crate::sessions::SignUpData;
use crate::vector3d::Vector3D;

#[derive(Clone, Debug, PartialEq)]
pub struct Player {
    id: GameObjectId,
    class: GameObjectRef,
    current_room: GameObjectRef,
    description: String,
    direction: Vector3D,
    gender: Gender,
    gold: u32,
    height: f32,
    hp: i16,
    inventory: Vec<GameObjectRef>,
    is_admin: bool,
    mp: i16,
    name: String,
    password: String,
    race: GameObjectRef,
    session_id: Option<u64>,
    stats: CharacterStats,
    weight: f32,
}

impl Player {
    pub fn hydrate(id: GameObjectId, json: &str) -> Result<SharedGameObject, String> {
        match serde_json::from_str::<PlayerDto>(json) {
            Ok(player_dto) => Ok(SharedGameObject::new(Self {
                id,
                class: player_dto.class,
                current_room: player_dto.currentRoom,
                description: player_dto.description,
                direction: player_dto.direction.unwrap_or_default(),
                gender: Gender::hydrate(&player_dto.gender),
                gold: player_dto.gold,
                height: player_dto.height,
                hp: player_dto.hp,
                inventory: player_dto.inventory.unwrap_or_default(),
                is_admin: player_dto.isAdmin.unwrap_or(false),
                mp: player_dto.mp,
                name: player_dto.name,
                password: player_dto.password,
                race: player_dto.race,
                session_id: None,
                stats: player_dto.stats,
                weight: player_dto.weight,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
    }

    pub fn is_admin(&self) -> bool {
        self.is_admin
    }

    pub fn matches_password(&self, password: &str) -> bool {
        match pbkdf2_check(password, &self.password) {
            Ok(()) => true,
            _ => false,
        }
    }

    pub fn new(id: GameObjectId, sign_up_data: &SignUpData) -> (Self, bool) {
        let class = sign_up_data.class.clone().unwrap();
        let race = sign_up_data.race.clone().unwrap();
        Self {
            id,
            class: class.object_ref(),
            current_room: race.starting_room(),
            description: String::new(),
            direction: Vector3D::new(0, 0, 0),
            gender: sign_up_data.gender.clone(),
            gold: sign_up_data.gold,
            height: sign_up_data.height,
            hp: sign_up_data.stats.max_hp(),
            inventory: Vec::new(),
            is_admin: false,
            mp: sign_up_data.stats.max_mp(),
            name: sign_up_data.user_name.clone(),
            password: String::new(),
            race: race.object_ref(),
            session_id: None,
            stats: sign_up_data.stats.clone(),
            weight: sign_up_data.weight,
        }
        .with_password(&sign_up_data.password)
    }

    pub fn session_id(&self) -> Option<u64> {
        self.session_id
    }

    pub fn with_admin(&self, admin: bool) -> (Self, bool) {
        (
            Self {
                is_admin: admin,
                ..self.clone()
            },
            true,
        )
    }

    pub fn with_password(&self, password: &str) -> (Self, bool) {
        (
            match pbkdf2_simple(password, 10) {
                Ok(password) => Self {
                    password,
                    ..self.clone()
                },
                Err(error) => panic!("Cannot create password hash: {:?}", error),
            },
            true,
        )
    }

    pub fn with_session_id(&self, session_id: Option<u64>) -> (Self, bool) {
        (
            Self {
                session_id,
                ..self.clone()
            },
            false,
        )
    }
}

impl Character for Player {
    fn class(&self) -> Option<GameObjectRef> {
        Some(self.class)
    }

    fn current_room(&self) -> GameObjectRef {
        self.current_room
    }

    fn direction(&self) -> &Vector3D {
        &self.direction
    }

    fn gender(&self) -> Gender {
        self.gender
    }

    fn gold(&self) -> u32 {
        self.gold
    }

    fn height(&self) -> f32 {
        self.height
    }

    fn hp(&self) -> i16 {
        self.hp
    }

    fn inventory(&self) -> &Vec<GameObjectRef> {
        &self.inventory
    }

    fn max_hp(&self) -> i16 {
        self.stats.max_hp()
    }

    fn max_mp(&self) -> i16 {
        self.stats.max_mp()
    }

    fn mp(&self) -> i16 {
        self.mp
    }

    fn race(&self) -> GameObjectRef {
        self.race
    }

    fn stats(&self) -> &CharacterStats {
        &self.stats
    }

    fn weight(&self) -> f32 {
        self.weight
    }

    fn with_current_room(&self, room: GameObjectRef) -> (SharedGameObject, bool) {
        (
            SharedGameObject::new(Self {
                current_room: room,
                ..self.clone()
            }),
            true,
        )
    }

    fn with_direction(&self, direction: Vector3D) -> (SharedGameObject, bool) {
        (
            SharedGameObject::new(Self {
                direction,
                ..self.clone()
            }),
            true,
        )
    }
}

impl fmt::Display for Player {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Player({}, {})", self.id, self.name)
    }
}

impl GameObject for Player {
    fn as_character(&self) -> Option<&dyn Character> {
        Some(self)
    }

    fn as_player(&self) -> Option<&Self> {
        Some(&self)
    }

    fn description(&self) -> &str {
        &self.description
    }

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Player
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn serialize(&self) -> String {
        serde_json::to_string_pretty(&PlayerDto {
            class: self.class,
            currentRoom: self.current_room,
            description: self.description.clone(),
            direction: Some(self.direction.clone()),
            gender: self.gender.serialize(),
            gold: self.gold,
            height: self.height,
            hp: self.hp,
            inventory: if self.inventory.is_empty() {
                None
            } else {
                Some(self.inventory.clone())
            },
            isAdmin: Some(self.is_admin),
            mp: self.mp,
            name: self.name.clone(),
            password: self.password.clone(),
            race: self.race,
            stats: self.stats.clone(),
            weight: self.weight,
        })
        .unwrap_or_else(|error| {
            panic!(
                "Failed to serialize object {:?}: {:?}",
                self.object_ref(),
                error
            )
        })
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct PlayerDto {
    class: GameObjectRef,
    currentRoom: GameObjectRef,
    description: String,
    direction: Option<Vector3D>,
    gender: Option<String>,
    gold: u32,
    height: f32,
    hp: i16,
    inventory: Option<Vec<GameObjectRef>>,
    isAdmin: Option<bool>,
    mp: i16,
    name: String,
    password: String,
    race: GameObjectRef,
    stats: CharacterStats,
    weight: f32,
}
