use crate::commands::CommandHelpers;
use crate::objects::Realm;

use super::api_request_processor::{ApiReply, ApiRequestProcessor};

/// Lists all the triggers.
pub fn triggers_list(_: &mut Realm, mut helpers: CommandHelpers) -> Result<ApiReply, ApiReply> {
    let signatures = helpers.trigger_registry.signatures();
    let processor = ApiRequestProcessor::try_new(&mut helpers)?;
    Ok(processor.reply(signatures))
}
