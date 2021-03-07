use serde::Serialize;

use crate::commands::CommandHelpers;
use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;

use super::api_request_processor::ApiRequestProcessor;

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
    _: &mut Realm,
    player_ref: GameObjectRef,
    mut helpers: CommandHelpers,
) -> Vec<PlayerOutput> {
    let mut output: Vec<PlayerOutput> = Vec::new();

    let signatures = helpers.trigger_registry.signatures();
    if let Some(mut processor) = ApiRequestProcessor::try_new(&mut output, player_ref, &mut helpers)
    {
        processor.send_reply(signatures);
    }

    output
}
