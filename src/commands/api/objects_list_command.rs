use serde_json::json;

use crate::objects::Realm;
use crate::{commands::CommandHelpers, game_object::GameObjectType};

use super::api_request_processor::{ApiReply, ApiRequestProcessor};

/// Lists all the objects of a given type.
pub fn objects_list(realm: &mut Realm, mut helpers: CommandHelpers) -> Result<ApiReply, ApiReply> {
    let mut processor = ApiRequestProcessor::try_new(&mut helpers)?;

    let object_type_string = processor.take_word("Missing object type")?;
    let object_type = GameObjectType::from_str(&object_type_string)
        .map_err(|message| processor.error_reply(400, &message))?;

    let objects: Vec<serde_json::Value> = realm
        .objects_of_type(object_type)
        .map(|object| {
            let mut hydrated_object = object.dehydrate();
            if let Some(json_object) = hydrated_object.as_object_mut() {
                json_object.insert("id".to_owned(), json!(object.id()));
            }
            hydrated_object
        })
        .collect();

    Ok(processor.reply(objects))
}
