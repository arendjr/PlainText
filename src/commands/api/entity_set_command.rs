use super::api_request_processor::{ApiReply, ApiRequestProcessor};
use crate::commands::CommandHelpers;
use crate::entity::hydrate;
use crate::entity::Realm;

/// Sets all properties of the referenced entity to the specified JSON.
pub fn entity_set(realm: &mut Realm, mut helpers: CommandHelpers) -> Result<ApiReply, ApiReply> {
    let mut processor = ApiRequestProcessor::try_new(&mut helpers)?;
    let entity_ref = processor.take_entity_ref()?;
    let content = processor.take_rest();

    let entity =
        hydrate(entity_ref, &content).map_err(|message| processor.error_reply(400, &message))?;
    realm.set(entity_ref, entity);

    Ok(processor.success_reply())
}
