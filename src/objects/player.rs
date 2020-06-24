use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;
use std::sync::Arc;

use crate::character_stats::CharacterStats;
use crate::game_object::{GameObject, GameObjectId, GameObjectRef, GameObjectType};

#[derive(Debug)]
pub struct Player {
    id: GameObjectId,
    current_room: GameObjectRef,
    description: String,
    height: f32,
    name: String,
    race: GameObjectRef,
    stats: CharacterStats,
    weight: f32,
}

impl Player {
    pub fn get_current_room(&self) -> GameObjectRef {
        self.current_room
    }

    pub fn hydrate(id: GameObjectId, json: &str) -> Result<Arc<dyn GameObject>, String> {
        match serde_json::from_str::<PlayerDto>(json) {
            Ok(player_dto) => Ok(Arc::new(Player {
                id,
                current_room: player_dto.currentRoom,
                description: player_dto.description,
                height: player_dto.height,
                name: player_dto.name,
                race: player_dto.race,
                stats: player_dto.stats,
                weight: player_dto.weight,
            })),
            Err(error) => Err(format!("parse error: {}", error)),
        }
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

    fn get_name(&self) -> String {
        self.name.clone()
    }

    fn to_player(&self) -> Option<&Player> {
        Some(self)
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct PlayerDto {
    cost: f32,
    currentRoom: GameObjectRef,
    description: String,
    height: f32,
    name: String,
    race: GameObjectRef,
    stats: CharacterStats,
    weight: f32,
}
