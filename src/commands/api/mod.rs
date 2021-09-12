use crate::commands::CommandHelpers;
use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::sessions::SessionOutput;

mod api_request_processor;
mod object_create_command;
mod object_delete_command;
mod object_set_command;
mod objects_list_command;
mod property_set_command;
mod triggers_list_command;

pub use object_create_command::object_create;
pub use object_delete_command::object_delete;
pub use object_set_command::object_set;
pub use objects_list_command::objects_list;
pub use property_set_command::property_set;
pub use triggers_list_command::triggers_list;

use self::api_request_processor::ApiReply;

pub fn wrap_api_request<F>(
    f: F,
) -> Box<dyn Fn(&mut Realm, GameObjectRef, CommandHelpers) -> Vec<PlayerOutput>>
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
