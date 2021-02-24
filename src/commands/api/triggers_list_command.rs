use serde::Serialize;

use crate::commands::CommandHelpers;
use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::SessionOutput;

#[derive(Serialize)]
#[serde(rename_all = "camelCase")]
struct ApiReply {
    request_id: String,
    error_code: u32,
    error_message: String,
    data: serde_json::Value,
}

/// Lists all the triggers.
pub fn triggers_list(
    realm: Realm,
    player_ref: GameObjectRef,
    helpers: CommandHelpers,
) -> (Realm, Vec<PlayerOutput>) {
    let processor = helpers.command_line_processor;

    let mut output: Vec<PlayerOutput> = Vec::new();

    processor.take_word();

    match processor.take_word() {
        Some(request_id) => {
            let data = serde_json::to_value(helpers.trigger_registry.signatures()).unwrap();

            let value = serde_json::to_value(ApiReply {
                request_id,
                error_code: 0,
                error_message: "".to_owned(),
                data,
            })
            .unwrap();

            push_session_output!(output, player_ref, SessionOutput::JSON(value));
        }
        None => {
            let value = serde_json::to_value(ApiReply {
                request_id: "".to_owned(),
                error_code: 400,
                error_message: "Missing request ID".to_owned(),
                data: serde_json::Value::Null,
            })
            .unwrap();

            push_session_output!(output, player_ref, SessionOutput::JSON(value));
        }
    }

    (realm, output)
}
