use super::api_request_processor::{ApiReply, ApiRequestProcessor};
use crate::{
    commands::CommandHelpers,
    entity::{hydrate, EntityRef, EntityType, Realm},
};

/// Creates a new entity from the specified JSON.
pub fn entity_create(realm: &mut Realm, mut helpers: CommandHelpers) -> Result<ApiReply, ApiReply> {
    let mut processor = ApiRequestProcessor::try_new(&mut helpers)?;

    let entity_type_string = processor.take_word("Missing entity type")?;
    let entity_type = EntityType::from_str(&entity_type_string)
        .map_err(|message| processor.error_reply(400, &message))?;

    let entity_ref = EntityRef::new(entity_type, realm.next_id());
    let content = processor.take_rest();

    let entity =
        hydrate(entity_ref, &content).map_err(|message| processor.error_reply(400, &message))?;
    realm.set(entity_ref, entity);

    Ok(processor.reply(entity_ref))
}
