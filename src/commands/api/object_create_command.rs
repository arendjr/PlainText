use crate::commands::CommandHelpers;
use crate::game_object::{hydrate, GameObjectRef, GameObjectType};
use crate::objects::Realm;

use super::api_request_processor::{ApiReply, ApiRequestProcessor};

/// Creates a new object from the specified JSON.
pub fn object_create(realm: &mut Realm, mut helpers: CommandHelpers) -> Result<ApiReply, ApiReply> {
    let mut processor = ApiRequestProcessor::try_new(&mut helpers)?;

    let object_type_string = processor.take_word("Missing object type")?;
    let object_type = GameObjectType::from_str(&object_type_string)
        .map_err(|message| processor.error_reply(400, &message))?;

    let object_ref = GameObjectRef::new(object_type, realm.next_id());
    let content = processor.take_rest();

    let object =
        hydrate(object_ref, &content).map_err(|message| processor.error_reply(400, &message))?;
    realm.set(object_ref, object);

    Ok(processor.reply(object_ref))
}
