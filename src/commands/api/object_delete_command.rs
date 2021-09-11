use crate::commands::CommandHelpers;
use crate::objects::Realm;

use super::api_request_processor::{ApiReply, ApiRequestProcessor};

/// Deletes the referenced object.
pub fn object_delete(realm: &mut Realm, mut helpers: CommandHelpers) -> Result<ApiReply, ApiReply> {
    let mut processor = ApiRequestProcessor::try_new(&mut helpers)?;

    let object_ref = processor.take_object_ref()?;
    realm.unset(object_ref);

    Ok(processor.success_reply())
}
