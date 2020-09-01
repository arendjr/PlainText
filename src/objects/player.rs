use pbkdf2::{pbkdf2_check, pbkdf2_simple};
use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;
use std::sync::Arc;

use crate::character_stats::CharacterStats;
use crate::game_object::{GameObject, GameObjectId, GameObjectRef, GameObjectType};
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
    pub fn get_current_room(&self) -> GameObjectRef {
        self.current_room
    }

    pub fn get_session_id(&self) -> Option<u64> {
        self.session_id
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Arc<dyn GameObject>, String> {
        match serde_json::from_str::<PlayerDto>(json) {
            Ok(player_dto) => Ok(Arc::new(Self {
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
        let mut player = Self {
            id,
            class: class.get_ref(),
            current_room: race.get_starting_room(),
            description: "".to_owned(),
            gender: sign_up_data.gender.clone(),
            height: sign_up_data.height,
            name: sign_up_data.user_name.clone(),
            password: String::new(),
            race: race.get_ref(),
            session_id: None,
            stats: sign_up_data.stats.clone(),
            weight: sign_up_data.weight,
        };
        player.set_password(&sign_up_data.password);
        player
    }

    pub fn set_current_room(&mut self, room: GameObjectRef) {
        self.current_room = room;
    }

    pub fn set_password(&mut self, password: &str) {
        match pbkdf2_simple(password, 10) {
            Ok(password) => self.password = password,
            Err(error) => panic!("Cannot create password hash: {:?}", error),
        }
    }

    pub fn set_session_id(&mut self, session_id: Option<u64>) {
        self.session_id = session_id
    }
}

impl fmt::Display for Player {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Player({}, {})", self.id, self.name)
    }
}

impl GameObject for Player {
    fn get_id(&self) -> GameObjectId {
        self.id
    }

    fn get_object_type(&self) -> GameObjectType {
        GameObjectType::Player
    }

    fn get_name(&self) -> &str {
        &self.name
    }

    fn to_player(&self) -> Option<Self> {
        Some(self.clone())
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
