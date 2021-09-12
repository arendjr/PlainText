use serde::Serialize;
use serde_json::json;

use crate::{commands::command_helpers::CommandHelpers, game_object::GameObjectRef};

#[derive(Serialize)]
#[serde(rename_all = "camelCase")]
pub struct ApiReply {
    request_id: String,
    error_code: u32,
    error_message: String,
    data: serde_json::Value,
}

impl ApiReply {
    pub fn new(request_id: &str, data: serde_json::Value) -> Self {
        Self {
            request_id: request_id.to_owned(),
            error_code: 0,
            error_message: "".to_owned(),
            data,
        }
    }

    pub fn new_error(request_id: &str, error_code: u32, error_message: &str) -> Self {
        Self {
            request_id: request_id.to_owned(),
            error_code,
            error_message: error_message.to_owned(),
            data: serde_json::Value::Null,
        }
    }
}

pub struct ApiRequestProcessor<'a, 'b> {
    helpers: &'a mut CommandHelpers<'b>,
    request_id: String,
}

impl<'a, 'b> ApiRequestProcessor<'a, 'b> {
    pub fn error_reply(&self, error_code: u32, error_message: &str) -> ApiReply {
        ApiReply::new_error(&self.request_id, error_code, error_message)
    }

    pub fn reply<T>(&self, data: T) -> ApiReply
    where
        T: Serialize,
    {
        ApiReply::new(&self.request_id, serde_json::to_value(data).unwrap())
    }

    pub fn success_reply(&self) -> ApiReply {
        self.reply(json!({"success": true}))
    }

    pub fn take_object_ref(&mut self) -> Result<GameObjectRef, ApiReply> {
        self.take_optional_word()
            .ok_or_else(|| ApiReply::new_error(&self.request_id, 400, "No object ref given"))
            .and_then(|word| {
                GameObjectRef::from_str(&word).map_err(|error| self.error_reply(400, &error))
            })
    }

    pub fn take_optional_word(&mut self) -> Option<String> {
        self.helpers.command_line_processor.take_word()
    }

    pub fn take_rest(&mut self) -> String {
        self.helpers.command_line_processor.take_rest()
    }

    pub fn take_word(&mut self, message: &str) -> Result<String, ApiReply> {
        self.helpers
            .command_line_processor
            .take_word()
            .ok_or_else(|| self.error_reply(400, message))
    }

    pub fn try_new(helpers: &'a mut CommandHelpers<'b>) -> Result<Self, ApiReply> {
        let processor = &mut helpers.command_line_processor;

        processor.take_word(); // alias

        match processor.take_word() {
            Some(request_id) => Ok(Self {
                helpers,
                request_id,
            }),
            None => Err(ApiReply::new_error("", 400, "Missing request ID")),
        }
    }
}
