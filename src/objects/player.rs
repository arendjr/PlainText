use pbkdf2::{pbkdf2_check, pbkdf2_simple};
use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::character_stats::CharacterStats;
use crate::game_object::{
    GameObject, GameObjectId, GameObjectRef, GameObjectType, SharedGameObject,
};
use crate::sessions::SignUpData;

#[derive(Clone, Debug, PartialEq)]
pub enum Gender {
    Unspecified,
    Male,
    Female,
}

#[derive(Clone, Debug, PartialEq)]
pub struct Player {
    id: GameObjectId,
    class: GameObjectRef,
    current_room: GameObjectRef,
    description: String,
    gender: Gender,
    height: f32,
    name: String,
    password: String,
    race: GameObjectRef,
    session_id: Option<u64>,
    stats: CharacterStats,
    weight: f32,
}

impl Player {
    pub fn current_room(&self) -> GameObjectRef {
        self.current_room
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<SharedGameObject, String> {
        match serde_json::from_str::<PlayerDto>(json) {
            Ok(player_dto) => Ok(SharedGameObject::new(Self {
                id,
                class: player_dto.class,
                current_room: player_dto.currentRoom,
                description: player_dto.description,
                gender: match &player_dto.gender.as_ref().map(String::as_str) {
                    Some("male") => Gender::Male,
                    Some("female") => Gender::Female,
                    _ => Gender::Unspecified,
                },
                height: player_dto.height,
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

    pub fn matches_password(&self, password: &str) -> bool {
        match pbkdf2_check(password, &self.password) {
            Ok(()) => true,
            _ => false,
        }
    }

    pub fn new(id: GameObjectId, sign_up_data: &SignUpData) -> Self {
        let class = sign_up_data.class.clone().unwrap();
        let race = sign_up_data.race.clone().unwrap();
        Self {
            id,
            class: class.object_ref(),
            current_room: race.starting_room(),
            description: "".to_owned(),
            gender: sign_up_data.gender.clone(),
            height: sign_up_data.height,
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

    pub fn with_current_room(&self, room: GameObjectRef) -> Self {
        Self {
            current_room: room,
            ..self.clone()
        }
    }

    pub fn with_password(&self, password: &str) -> Self {
        match pbkdf2_simple(password, 10) {
            Ok(password) => Self {
                password,
                ..self.clone()
            },
            Err(error) => panic!("Cannot create password hash: {:?}", error),
        }
    }

    pub fn with_session_id(&self, session_id: Option<u64>) -> Self {
        Self {
            session_id,
            ..self.clone()
        }
    }
}

impl fmt::Display for Player {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Player({}, {})", self.id, self.name)
    }
}

impl GameObject for Player {
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
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct PlayerDto {
    class: GameObjectRef,
    cost: f32,
    currentRoom: GameObjectRef,
    description: String,
    gender: Option<String>,
    height: f32,
    name: String,
    password: String,
    race: GameObjectRef,
    stats: CharacterStats,
    weight: f32,
}
