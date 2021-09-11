use crate::commands::CommandHelpers;
use crate::objects::Realm;

use super::api_request_processor::{ApiReply, ApiRequestProcessor};

/// Lists all the objects of a given type.
pub fn property_set(realm: &mut Realm, mut helpers: CommandHelpers) -> Result<ApiReply, ApiReply> {
    let mut processor = ApiRequestProcessor::try_new(&mut helpers)?;

    let object_ref = processor.take_object_ref()?;
    let object = realm
        .object_mut(object_ref)
        .ok_or_else(|| processor.error_reply(404, &format!("Unknown object: {}", object_ref)))?;

    let prop_name = processor.take_word("No property name given")?;
    let value = processor.take_rest();

    match object.set_property(&prop_name, &value) {
        Ok(()) => Ok(processor.success_reply()),
        Err(error) => Err(processor.error_reply(400, &error)),
    }
}
