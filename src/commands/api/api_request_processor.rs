use serde::Serialize;
use serde_json::json;

use crate::{
    commands::command_helpers::CommandHelpers, game_object::GameObjectRef,
    player_output::PlayerOutput, SessionOutput,
};

#[derive(Serialize)]
#[serde(rename_all = "camelCase")]
pub struct ApiReply {
    request_id: String,
    error_code: u32,
    error_message: String,
    data: serde_json::Value,
}

pub struct ApiRequestProcessor<'a, 'b> {
    output: &'a mut Vec<PlayerOutput>,
    player_ref: GameObjectRef,
    helpers: &'a mut CommandHelpers<'b>,
    request_id: String,
}

impl<'a, 'b> ApiRequestProcessor<'a, 'b> {
    pub fn send_error(&mut self, error_code: u32, error_message: String) {
        let reply = serde_json::to_value(ApiReply {
            request_id: self.request_id.clone(),
            error_code,
            error_message,
            data: serde_json::Value::Null,
        })
        .unwrap();

        push_session_output!(self.output, self.player_ref, SessionOutput::JSON(reply));
    }

    pub fn send_reply<T>(&mut self, data: T)
    where
        T: Serialize,
    {
        let reply = serde_json::to_value(ApiReply {
            request_id: self.request_id.clone(),
            error_code: 0,
            error_message: "".to_owned(),
            data: serde_json::to_value(data).unwrap(),
        })
        .unwrap();

        push_session_output!(self.output, self.player_ref, SessionOutput::JSON(reply));
    }

    pub fn send_success_reply(&mut self) {
        self.send_reply(json!({"success": true}));
    }

    pub fn take_object_ref(&mut self) -> Result<GameObjectRef, String> {
        self.take_word()
            .ok_or_else(|| "No object ref given".to_owned())
            .and_then(|word| GameObjectRef::from_str(&word))
    }

    pub fn take_rest(&mut self) -> String {
        self.helpers.command_line_processor.take_rest()
    }

    pub fn take_word(&mut self) -> Option<String> {
        self.helpers.command_line_processor.take_word()
    }

    pub fn try_new(
        output: &'a mut Vec<PlayerOutput>,
        player_ref: GameObjectRef,
        helpers: &'a mut CommandHelpers<'b>,
    ) -> Option<Self> {
        let processor = &mut helpers.command_line_processor;

        processor.take_word(); // alias

        match processor.take_word() {
            Some(request_id) => Some(Self {
                helpers,
                output,
                player_ref,
                request_id,
            }),
            None => {
                let reply = serde_json::to_value(ApiReply {
                    request_id: "".to_owned(),
                    error_code: 400,
                    error_message: "Missing request ID".to_owned(),
                    data: serde_json::Value::Null,
                })
                .unwrap();

                push_session_output!(output, player_ref, SessionOutput::JSON(reply));

                None
            }
        }
    }
}
