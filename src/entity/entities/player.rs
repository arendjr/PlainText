use crate::{
    entity::{Character, Entity, EntityId, EntityPersistence, EntityRef, EntityType, StatsItem},
    entity_copy_prop, entity_string_prop,
    sessions::SignUpData,
};
use pbkdf2::{pbkdf2_check, pbkdf2_simple};
use serde::{Deserialize, Serialize};

#[derive(Clone, Debug, Deserialize, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Player {
    #[serde(skip)]
    id: EntityId,

    #[serde(flatten)]
    pub character: Character,

    description: String,

    #[serde(default)]
    is_admin: bool,

    name: String,

    #[serde(skip)]
    needs_sync: bool,

    password: String,

    #[serde(skip)]
    session_id: Option<u64>,

    #[serde(flatten)]
    stats_item: StatsItem,
}

impl Player {
    entity_copy_prop!(pub, is_admin, set_is_admin, bool);
    entity_copy_prop!(
        pub,
        session_id,
        set_session_id,
        Option<u64>,
        EntityPersistence::DontSync
    );

    pub fn hydrate(id: EntityId, json: &str) -> Result<Box<dyn Entity>, String> {
        let mut player = serde_json::from_str::<Player>(json)
            .map_err(|error| format!("parse error: {}", error))?;
        player.id = id;
        Ok(Box::new(player))
    }

    pub fn matches_password(&self, password: &str) -> bool {
        matches!(pbkdf2_check(password, &self.password), Ok(()))
    }

    pub fn new(id: EntityId, sign_up_data: &SignUpData) -> Self {
        let character = Character::from_sign_up_data(sign_up_data);
        Self {
            id,
            character,
            description: String::new(),
            is_admin: false,
            name: sign_up_data.user_name.clone(),
            needs_sync: true,
            password: match pbkdf2_simple(&sign_up_data.password, 10) {
                Ok(password) => password,
                Err(error) => panic!("Cannot create password hash: {:?}", error),
            },
            session_id: None,
            stats_item: StatsItem::from_stats(sign_up_data.stats.clone()),
        }
    }

    pub fn set_password(&mut self, password: &str) {
        match pbkdf2_simple(password, 10) {
            Ok(password) => {
                self.password = password;
                self.set_needs_sync(true);
            }
            Err(error) => panic!("Cannot create password hash: {:?}", error),
        }
    }
}

impl Entity for Player {
    entity_string_prop!(name, set_name);
    entity_string_prop!(description, set_description);

    fn as_character(&self) -> Option<&Character> {
        Some(&self.character)
    }

    fn as_character_mut(&mut self) -> Option<&mut Character> {
        Some(&mut self.character)
    }

    fn as_player(&self) -> Option<&Self> {
        Some(self)
    }

    fn as_player_mut(&mut self) -> Option<&mut Self> {
        Some(self)
    }

    fn dehydrate(&self) -> String {
        serde_json::to_string_pretty(self).unwrap_or_else(|error| {
            panic!(
                "Failed to serialize entity {:?}: {:?}",
                self.entity_ref(),
                error
            )
        })
    }

    fn entity_ref(&self) -> EntityRef {
        EntityRef::new(EntityType::Player, self.id)
    }

    fn id(&self) -> EntityId {
        self.id
    }

    fn needs_sync(&self) -> bool {
        self.needs_sync || self.character.needs_sync() || self.stats_item.needs_sync()
    }

    fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;

        if !needs_sync {
            self.character.set_needs_sync(needs_sync);
            self.stats_item.set_needs_sync(needs_sync);
        }
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "description" => self.set_description(value.to_owned()),
            "isAdmin" => self.set_is_admin(value == "true"),
            "name" => self.set_name(value.to_owned()),
            "password" => self.set_password(value),
            _ => {
                return self
                    .character
                    .set_property(prop_name, value)
                    .or_else(|_| self.stats_item.set_property(prop_name, value))
            }
        }
        Ok(())
    }

    fn to_json_value(&self) -> serde_json::Result<serde_json::Value> {
        serde_json::to_value(self)
    }
}
