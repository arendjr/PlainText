use serde::de::{self, Deserialize, Deserializer, Visitor};
use serde::ser::{Serialize, Serializer};
use std::ffi::OsString;
use std::fmt;

pub type GameObjectId = u32;

#[derive(Clone, Copy, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub enum GameObjectType {
    Class,
    //Container,
    //Group,
    Item,
    Npc,
    Player,
    Portal,
    Race,
    Realm,
    Room,
    //Shield,
    //Weapon,
}

impl fmt::Display for GameObjectType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.write_str(match self {
            GameObjectType::Class => "class",
            //GameObjectType::Container => "container",
            //GameObjectType::Group => "group",
            GameObjectType::Item => "item",
            GameObjectType::Npc => "character",
            GameObjectType::Player => "player",
            GameObjectType::Portal => "portal",
            GameObjectType::Race => "race",
            GameObjectType::Realm => "realm",
            GameObjectType::Room => "room",
            //GameObjectType::Shield => "shield",
            //GameObjectType::Weapon => "weapon",
        })
    }
}

impl GameObjectType {
    pub fn from_str(object_type_str: &str) -> Result<GameObjectType, String> {
        match object_type_str {
            "character" => Ok(GameObjectType::Npc),
            "class" => Ok(GameObjectType::Class),
            "item" => Ok(GameObjectType::Item),
            "player" => Ok(GameObjectType::Player),
            "portal" => Ok(GameObjectType::Portal),
            "race" => Ok(GameObjectType::Race),
            "room" => Ok(GameObjectType::Room),
            _ => Err(format!("Unknown game object type: \"{}\"", object_type_str)),
        }
    }
}

#[derive(Clone, Copy, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub struct GameObjectRef(pub GameObjectType, pub GameObjectId);

impl GameObjectRef {
    pub fn from_file_name(file_name: &OsString) -> Option<Self> {
        let file_name = file_name.to_str()?;
        let split = file_name.split('.').collect::<Vec<&str>>();
        if split.len() != 2 {
            return None;
        }

        let object_type = GameObjectType::from_str(split[0]).ok()?;
        let id = split[1].parse::<GameObjectId>();
        match id {
            Ok(id) => Some(Self(object_type, id)),
            Err(_) => None,
        }
    }

    pub fn from_str(ref_str: &str) -> Result<Self, String> {
        let split = ref_str.split(':').collect::<Vec<&str>>();
        if split.len() != 2 {
            return Err("Invalid game object ref".to_owned());
        }

        let object_type = GameObjectType::from_str(split[0])?;
        let id = split[1].parse::<GameObjectId>();
        match id {
            Ok(id) => Ok(Self(object_type, id)),
            Err(_) => Err("Invalid game object ref".to_owned()),
        }
    }

    pub fn id(&self) -> GameObjectId {
        self.1
    }

    pub fn object_type(&self) -> GameObjectType {
        self.0
    }

    /// Returns the only object ref from a vector, if the vector only has a single item.
    pub fn only(vec: &Vec<GameObjectRef>) -> Option<GameObjectRef> {
        if vec.len() == 1 {
            vec.first().map(|&object_ref| object_ref)
        } else {
            None
        }
    }

    pub fn to_file_name(&self) -> String {
        format!("{}.{:09}", self.0, self.1)
    }

    pub fn vec_from_str(list_str: &str) -> Result<Vec<Self>, String> {
        if !list_str.starts_with('[') || !list_str.ends_with(']') {
            return Err("Invalid stats -- use square brackets to denote stats".to_owned());
        }

        list_str[1..list_str.len() - 1]
            .split(',')
            .map(|ref_str| GameObjectRef::from_str(ref_str))
            .collect()
    }
}

impl<'de> Deserialize<'de> for GameObjectRef {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        deserializer.deserialize_str(GameObjectRefVisitor)
    }
}

impl fmt::Display for GameObjectRef {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}:{}", self.0, self.1)
    }
}

impl Serialize for GameObjectRef {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        serializer.serialize_str(&format!("{}", self))
    }
}

struct GameObjectRefVisitor;

impl<'de> Visitor<'de> for GameObjectRefVisitor {
    type Value = GameObjectRef;

    fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        formatter.write_str("a string with type and id")
    }

    fn visit_str<E>(self, value: &str) -> Result<Self::Value, E>
    where
        E: de::Error,
    {
        let split = value.split(':').collect::<Vec<&str>>();
        if split.len() != 2 {
            return Err(E::custom(
                "refs must have a type and id separated by a colon",
            ));
        }

        let object_type = GameObjectType::from_str(split[0]);
        let id = split[1].parse::<GameObjectId>();
        match (object_type, id) {
            (Ok(object_type), Ok(id)) => Ok(GameObjectRef(object_type, id)),
            _ => Err(E::custom(format!("invalid ref: {}", value))),
        }
    }
}
