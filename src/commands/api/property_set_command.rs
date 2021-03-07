use crate::commands::CommandHelpers;
use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;

use super::api_request_processor::ApiRequestProcessor;

/// Lists all the objects of a given type.
pub fn property_set(
    realm: &mut Realm,
    player_ref: GameObjectRef,
    mut helpers: CommandHelpers,
) -> Vec<PlayerOutput> {
    let mut output: Vec<PlayerOutput> = Vec::new();

    let mut processor = unwrap_or_return!(
        ApiRequestProcessor::try_new(&mut output, player_ref, &mut helpers),
        output
    );

    let object_ref = match processor.take_object_ref() {
        Ok(object_ref) => object_ref,
        Err(error) => {
            processor.send_error(400, error);
            return output;
        }
    };

    let object = match realm.object_mut(object_ref) {
        Some(object) => object,
        None => {
            processor.send_error(404, format!("Unknown object: {}", object_ref));
            return output;
        }
    };

    let prop_name = match processor.take_word() {
        Some(prop_name) => prop_name,
        None => {
            processor.send_error(400, "No property name given".to_owned());
            return output;
        }
    };

    let value = processor.take_rest();

    match object.set_property(&prop_name, &value) {
        Ok(()) => processor.send_success_reply(),
        Err(error) => processor.send_error(400, error),
    }

    output
}
