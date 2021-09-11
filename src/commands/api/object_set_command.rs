use crate::commands::CommandHelpers;
use crate::game_object::hydrate;
use crate::objects::Realm;

use super::api_request_processor::{ApiReply, ApiRequestProcessor};

/// Sets all properties of the referenced object to the specified JSON.
pub fn object_set(realm: &mut Realm, mut helpers: CommandHelpers) -> Result<ApiReply, ApiReply> {
    let mut processor = ApiRequestProcessor::try_new(&mut helpers)?;
    let object_ref = processor.take_object_ref()?;
    let content = processor.take_rest();

    let object =
        hydrate(object_ref, &content).map_err(|message| processor.error_reply(400, &message))?;
    realm.set(object_ref, object);

    Ok(processor.success_reply())
}
