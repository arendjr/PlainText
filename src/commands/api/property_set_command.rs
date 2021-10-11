use super::api_request_processor::{ApiReply, ApiRequestProcessor};
use crate::commands::CommandHelpers;
use crate::entity::Realm;

/// Sets a single property on an entity.
pub fn property_set(realm: &mut Realm, mut helpers: CommandHelpers) -> Result<ApiReply, ApiReply> {
    let mut processor = ApiRequestProcessor::try_new(&mut helpers)?;

    let entity_ref = processor.take_entity_ref()?;
    let entity = realm
        .entity_mut(entity_ref)
        .ok_or_else(|| processor.error_reply(404, &format!("Unknown entity: {}", entity_ref)))?;

    let prop_name = processor.take_word("No property name given")?;
    let value = processor.take_rest();

    match entity.set_property(&prop_name, &value) {
        Ok(()) => Ok(processor.success_reply()),
        Err(error) => Err(processor.error_reply(400, &error)),
    }
}
