use crate::{entity::EntityPersistence, entity_copy_prop, entity_string_prop};
use serde::{Deserialize, Serialize};
use std::num::NonZeroU32;

#[derive(Clone, Debug, Deserialize, PartialEq, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Openable {
    #[serde(default, skip_serializing_if = "String::is_empty")]
    auto_close_message: String,
    #[serde(default, skip_serializing_if = "Option::is_none")]
    auto_close_timeout: Option<NonZeroU32>,
    #[serde(skip)]
    is_open: bool,
    #[serde(skip)]
    needs_sync: bool,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    required_key: String,
}

impl Openable {
    entity_string_prop!(pub, auto_close_message, set_auto_close_message);
    entity_copy_prop!(
        pub,
        auto_close_timeout,
        set_auto_close_timeout,
        Option<NonZeroU32>
    );
    entity_copy_prop!(pub, is_open, set_open, bool, EntityPersistence::DontSync);
    entity_string_prop!(pub, required_key, set_required_key);

    pub fn needs_sync(&self) -> bool {
        self.needs_sync
    }

    pub fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;
    }

    pub fn new() -> Self {
        Self {
            auto_close_message: String::new(),
            auto_close_timeout: None,
            is_open: false,
            needs_sync: false,
            required_key: String::new(),
        }
    }

    pub fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "auto_close_message" => self.set_auto_close_message(value.to_owned()),
            "auto_close_timeout" => self.set_auto_close_timeout(
                value
                    .parse::<u32>()
                    .map(NonZeroU32::new)
                    .map_err(|error| format!("Invalid timeout: {}", error))?,
            ),
            "is_open" => self.set_open(value == "true"),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }
}
