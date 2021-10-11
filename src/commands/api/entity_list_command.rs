use super::api_request_processor::{ApiReply, ApiRequestProcessor};
use crate::{commands::CommandHelpers, entity::EntityType, entity::Realm};
use serde_json::json;

/// Lists all the entities of a given type.
pub fn entity_list(realm: &mut Realm, mut helpers: CommandHelpers) -> Result<ApiReply, ApiReply> {
    let mut processor = ApiRequestProcessor::try_new(&mut helpers)?;

    let entity_type_string = processor.take_word("Missing entity type")?;
    let entity_type = EntityType::from_str(&entity_type_string)
        .map_err(|message| processor.error_reply(400, &message))?;

    let entities: Vec<serde_json::Value> = realm
        .entities_of_type(entity_type)
        .map(|entity| {
            let mut hydrated_entity = entity.to_json_value().expect("Could not serialize entity");
            if let Some(json_object) = hydrated_entity.as_object_mut() {
                json_object.insert("id".to_owned(), json!(entity.id()));
            }
            hydrated_entity
        })
        .collect();

    Ok(processor.reply(entities))
}
