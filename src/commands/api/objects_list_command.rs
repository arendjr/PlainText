use serde_json::json;

use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::{commands::CommandHelpers, game_object::GameObjectType};

use super::api_request_processor::ApiRequestProcessor;

/// Lists all the objects of a given type.
pub fn objects_list(
    realm: &mut Realm,
    player_ref: GameObjectRef,
    mut helpers: CommandHelpers,
) -> Vec<PlayerOutput> {
    let mut output: Vec<PlayerOutput> = Vec::new();

    let mut processor = unwrap_or_return!(
        ApiRequestProcessor::try_new(&mut output, player_ref, &mut helpers),
        output
    );

    let object_type_string = unwrap_or_return!(processor.take_word(), {
        processor.send_error(400, "Please specify an object type".to_owned());
        output
    });

    let object_type = match GameObjectType::from_str(&object_type_string) {
        Ok(object_type) => object_type,
        Err(error) => {
            processor.send_error(400, error);
            return output;
        }
    };

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
    output
}
