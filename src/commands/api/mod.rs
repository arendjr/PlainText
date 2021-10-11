use self::api_request_processor::ApiReply;
use crate::commands::CommandHelpers;
use crate::entity::EntityRef;
use crate::entity::Realm;
use crate::player_output::PlayerOutput;
use crate::sessions::SessionOutput;

mod api_request_processor;
mod entity_create_command;
mod entity_delete_command;
mod entity_list_command;
mod entity_set_command;
mod property_set_command;

pub use entity_create_command::entity_create;
pub use entity_delete_command::entity_delete;
pub use entity_list_command::entity_list;
pub use entity_set_command::entity_set;
pub use property_set_command::property_set;

pub fn wrap_api_request<F>(
    f: F,
) -> Box<dyn Fn(&mut Realm, EntityRef, CommandHelpers) -> Vec<PlayerOutput>>
where
    F: Fn(&mut Realm, CommandHelpers) -> Result<ApiReply, ApiReply> + 'static,
{
    Box::new(move |realm, player_ref, helpers| {
        let reply = match realm.player(player_ref) {
            Some(player) if player.is_admin() => match f(realm, helpers) {
                Ok(reply) => reply,
                Err(error) => error,
            },
            _ => ApiReply::new_error("", 403, "Forbidden"),
        };
        let reply = serde_json::to_value(reply).unwrap();

        let mut output: Vec<PlayerOutput> = Vec::new();
        push_session_output!(output, player_ref, SessionOutput::Json(reply));
        output
    })
}
