use serde_json::json;

use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::{commands::CommandHelpers, game_object::GameObjectType};

use super::api_request_processor::ApiRequestProcessor;

/// Lists all the objects of a given type.
pub fn objects_list(
    realm: Realm,
    player_ref: GameObjectRef,
    mut helpers: CommandHelpers,
) -> (Realm, Vec<PlayerOutput>) {
    let mut output: Vec<PlayerOutput> = Vec::new();

    if let Some(mut processor) = ApiRequestProcessor::try_new(&mut output, player_ref, &mut helpers)
    {
        if let Some(object_type_string) = processor.take_word() {
            if let Some(object_type) = GameObjectType::from_str(&object_type_string) {
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

                processor.send_reply(objects);
            } else {
                processor.send_error(400, format!("Unknown object type: {}", object_type_string));
            }
        } else {
            processor.send_error(400, "Please specify an object type".to_owned());
        }
    }

    (realm, output)
}
