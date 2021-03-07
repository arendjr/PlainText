use crate::commands::CommandHelpers;
use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;

use super::api_request_processor::ApiRequestProcessor;

/// Lists all the objects of a given type.
pub fn property_set(
    realm: Realm,
    player_ref: GameObjectRef,
    mut helpers: CommandHelpers,
) -> (Realm, Vec<PlayerOutput>) {
    let mut output: Vec<PlayerOutput> = Vec::new();

    let mut processor = unwrap_or_return!(
        ApiRequestProcessor::try_new(&mut output, player_ref, &mut helpers),
        (realm, output)
    );

    let object_ref = match processor.take_object_ref() {
        Ok(object_ref) => object_ref,
        Err(error) => {
            processor.send_error(400, error);
            return (realm, output);
        }
    };

    let object = match realm.object(object_ref) {
        Some(object) => object,
        None => {
            processor.send_error(404, format!("Unknown object: {}", object_ref));
            return (realm, output);
        }
    };

    let prop_name = match processor.take_word() {
        Some(prop_name) => prop_name,
        None => {
            processor.send_error(400, "No property name given".to_owned());
            return (realm, output);
        }
    };

    let value = processor.take_rest();

    match object.set_property(realm, &prop_name, &value) {
        Ok(new_realm) => {
            processor.send_success_reply();
            (new_realm, output)
        }
        Err(error) => {
            processor.send_error(400, error);
            (realm, output)
        }
    }
}
