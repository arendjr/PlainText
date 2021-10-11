use super::api_request_processor::{ApiReply, ApiRequestProcessor};
use crate::commands::CommandHelpers;
use crate::entity::Realm;

/// Deletes the referenced entity.
pub fn entity_delete(realm: &mut Realm, mut helpers: CommandHelpers) -> Result<ApiReply, ApiReply> {
    let mut processor = ApiRequestProcessor::try_new(&mut helpers)?;

    let entity_ref = processor.take_entity_ref()?;
    realm.unset(entity_ref);

    Ok(processor.success_reply())
}
