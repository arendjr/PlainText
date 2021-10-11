use serde::{
    de::{self, Deserialize, Deserializer, Visitor},
    ser::{Serialize, Serializer},
};
use std::{ffi::OsString, fmt};

pub type EntityId = u32;

#[derive(Clone, Copy, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub enum EntityType {
    Class,
    //Container,
    Group,
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

impl fmt::Display for EntityType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.write_str(match self {
            EntityType::Class => "class",
            //EntityType::Container => "container",
            EntityType::Group => "group",
            EntityType::Item => "item",
            EntityType::Npc => "character",
            EntityType::Player => "player",
            EntityType::Portal => "portal",
            EntityType::Race => "race",
            EntityType::Realm => "realm",
            EntityType::Room => "room",
            //EntityType::Shield => "shield",
            //EntityType::Weapon => "weapon",
        })
    }
}

impl EntityType {
    pub fn from_str(entity_type_str: &str) -> Result<EntityType, String> {
        match entity_type_str {
            "character" => Ok(EntityType::Npc),
            "class" => Ok(EntityType::Class),
            "group" => Ok(EntityType::Group),
            "item" => Ok(EntityType::Item),
            "player" => Ok(EntityType::Player),
            "portal" => Ok(EntityType::Portal),
            "race" => Ok(EntityType::Race),
            "room" => Ok(EntityType::Room),
            _ => Err(format!("Unknown entity type: \"{}\"", entity_type_str)),
        }
    }
}

#[derive(Clone, Copy, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub struct EntityRef(pub EntityType, pub EntityId);

impl EntityRef {
    pub fn new(entity_type: EntityType, id: EntityId) -> Self {
        Self(entity_type, id)
    }

    pub fn from_file_name(file_name: &OsString) -> Option<Self> {
        let file_name = file_name.to_str()?;
        let split = file_name.split('.').collect::<Vec<&str>>();
        if split.len() != 2 {
            return None;
        }

        let entity_type = EntityType::from_str(split[0]).ok()?;
        let id = split[1].parse::<EntityId>();
        match id {
            Ok(id) => Some(Self(entity_type, id)),
            Err(_) => None,
        }
    }

    pub fn from_str(ref_str: &str) -> Result<Self, String> {
        let split = ref_str.split(':').collect::<Vec<&str>>();
        if split.len() != 2 {
            return Err("Invalid entity ref".to_owned());
        }

        let entity_type = EntityType::from_str(split[0])?;
        let id = split[1].parse::<EntityId>();
        match id {
            Ok(id) => Ok(Self(entity_type, id)),
            Err(_) => Err("Invalid entity ref".to_owned()),
        }
    }

    pub fn id(&self) -> EntityId {
        self.1
    }

    pub fn entity_type(&self) -> EntityType {
        self.0
    }

    /// Returns the only entity ref from a vector, if the vector only has a single item.
    pub fn only(vec: &[EntityRef]) -> Option<EntityRef> {
        if vec.len() == 1 {
            vec.first().copied()
        } else {
            None
        }
    }

    pub fn to_file_name(self) -> String {
        format!("{}.{:09}", self.0, self.1)
    }

    pub fn vec_from_str(list_str: &str) -> Result<Vec<Self>, String> {
        if !list_str.starts_with('[') || !list_str.ends_with(']') {
            return Err("Invalid stats -- use square brackets to denote stats".to_owned());
        }

        list_str[1..list_str.len() - 1]
            .split(',')
            .map(|ref_str| EntityRef::from_str(ref_str))
            .collect()
    }
}

impl<'de> Deserialize<'de> for EntityRef {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        deserializer.deserialize_str(EntityRefVisitor)
    }
}

impl fmt::Display for EntityRef {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}:{}", self.0, self.1)
    }
}

impl Serialize for EntityRef {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        serializer.serialize_str(&format!("{}", self))
    }
}

struct EntityRefVisitor;

impl<'de> Visitor<'de> for EntityRefVisitor {
    type Value = EntityRef;

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

        let entity_type = EntityType::from_str(split[0]);
        let id = split[1].parse::<EntityId>();
        match (entity_type, id) {
            (Ok(entity_type), Ok(id)) => Ok(EntityRef(entity_type, id)),
            _ => Err(E::custom(format!("invalid ref: {}", value))),
        }
    }
}
