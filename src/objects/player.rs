use pbkdf2::{pbkdf2_check, pbkdf2_simple};
use serde::{Deserialize, Serialize};
use serde_json;
use std::fmt;

use crate::character_stats::CharacterStats;
use crate::game_object::{
    Character, GameObject, GameObjectId, GameObjectPersistence, GameObjectRef, GameObjectType,
    Gender, SharedGameObject,
};
use crate::sessions::SignUpData;
use crate::vector3d::Vector3D;

use super::Realm;

#[derive(Clone, Debug, PartialEq)]
pub struct Player {
    id: GameObjectId,
    class: Option<GameObjectRef>,
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
    game_object_copy_prop!(pub, is_admin, set_is_admin, bool);
    game_object_copy_prop!(
        pub,
        session_id,
        set_session_id,
        Option<u64>,
        GameObjectPersistence::DontSync
    );

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
            class: Some(class.object_ref()),
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
            password: match pbkdf2_simple(&sign_up_data.password, 10) {
                Ok(password) => password,
                Err(error) => panic!("Cannot create password hash: {:?}", error),
            },
            race: race.object_ref(),
            session_id: None,
            stats: sign_up_data.stats.clone(),
            weight: sign_up_data.weight,
        }
    }

    pub fn set_password(&self, realm: Realm, password: &str) -> Realm {
        match pbkdf2_simple(password, 10) {
            Ok(password) => realm.set_shared_object(
                self.object_ref(),
                SharedGameObject::new(Self {
                    password,
                    ..self.clone()
                }),
                GameObjectPersistence::Sync,
            ),
            Err(error) => panic!("Cannot create password hash: {:?}", error),
        }
    }
}

impl Character for Player {
    game_object_copy_prop!(class, set_class, Option<GameObjectRef>);
    game_object_copy_prop!(current_room, set_current_room, GameObjectRef);
    game_object_ref_prop!(direction, set_direction, Vector3D);
    game_object_copy_prop!(gender, set_gender, Gender);
    game_object_copy_prop!(gold, set_gold, u32);
    game_object_copy_prop!(height, set_height, f32);
    game_object_copy_prop!(hp, set_hp, i16);
    game_object_ref_prop!(inventory, set_inventory, Vec<GameObjectRef>);
    game_object_copy_prop!(mp, set_mp, i16);
    game_object_copy_prop!(race, set_race, GameObjectRef);
    game_object_ref_prop!(stats, set_stats, CharacterStats);
    game_object_copy_prop!(weight, set_weight, f32);
}

impl fmt::Display for Player {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Player({}, {})", self.id, self.name)
    }
}

impl GameObject for Player {
    game_object_string_prop!(name, set_name);
    game_object_string_prop!(description, set_description);

    fn as_character(&self) -> Option<&dyn Character> {
        Some(self)
    }

    fn as_player(&self) -> Option<&Self> {
        Some(&self)
    }

    fn dehydrate(&self) -> serde_json::Value {
        serde_json::to_value(PlayerDto {
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

    fn id(&self) -> GameObjectId {
        self.id
    }

    fn object_type(&self) -> GameObjectType {
        GameObjectType::Player
    }

    fn set_property(&self, realm: Realm, prop_name: &str, value: &str) -> Result<Realm, String> {
        match prop_name {
            "class" => Ok(self.set_class(realm, Some(GameObjectRef::from_str(value)?))),
            "current_room" => Ok(self.set_current_room(realm, GameObjectRef::from_str(value)?)),
            "description" => Ok(self.set_description(realm, value.to_owned())),
            "direction" => Ok(self.set_direction(realm, Vector3D::from_str(value)?)),
            "gender" => Ok(self.set_gender(realm, Gender::from_str(value)?)),
            "gold" => Ok(self.set_gold(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            "height" => Ok(self.set_height(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            "hp" => Ok(self.set_hp(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            "inventory" => Ok(self.set_inventory(realm, GameObjectRef::vec_from_str(value)?)),
            "isAdmin" => Ok(self.set_is_admin(realm, value == "true")),
            "mp" => Ok(self.set_mp(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            "name" => Ok(self.set_name(realm, value.to_owned())),
            "password" => Ok(self.set_password(realm, value)),
            "race" => Ok(self.set_race(realm, GameObjectRef::from_str(value)?)),
            "stats" => Ok(self.set_stats(realm, CharacterStats::from_str(value)?)),
            "weight" => Ok(self.set_weight(
                realm,
                value.parse().map_err(|error| format!("{:?}", error))?,
            )),
            _ => Err(format!("No property named \"{}\"", prop_name))?,
        }
    }
}

#[allow(non_snake_case)]
#[derive(Deserialize, Serialize)]
struct PlayerDto {
    class: Option<GameObjectRef>,
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
